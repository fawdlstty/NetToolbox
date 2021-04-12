#ifndef __PAGE_QPS_HPP__
#define __PAGE_QPS_HPP__

#include <vector>
#include <thread>
#include <tuple>
#include <chrono>
#include <string>
#include <functional>

#include "page_base.hpp"
#include "../tools/tool_String.hpp"
#include "../tools/tool_WebRequest.hpp"



class page_Qps: public page_base {
public:
	page_Qps (NetToolboxWnd *parent): page_base (parent) {
		_convert_data (false);
	}
	virtual ~page_Qps () = default;

	void ui_update_data () override {
		if (m_is_run) {
			size_t succ_count = 0, fail_count = 0;
			for (size_t i = 0; i < m_succ_counts.size (); ++i) {
				succ_count += m_succ_counts[i];
				fail_count += m_fail_counts[i];
			}
			size_t inc_succ_count = succ_count - m_last_succ_count, inc_fail_count = fail_count - m_last_fail_count;
			m_desp += fmt::format (_IT (_T ("Sended {:-6}，Succeed {:-6}，Failed {}\n")).data (), inc_succ_count + inc_fail_count, inc_succ_count, inc_fail_count);
			m_qps_content->SetText (m_desp.data ());
			m_last_succ_count += inc_succ_count;
			m_last_fail_count += inc_fail_count;
		}
	}

	bool OnClick (TNotifyUI& msg) override {
		 faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("qps_begin") || name == _T ("qps_stop")) {
			if ((name == _T ("qps_begin")) == m_is_run)
				return true;
			if (!m_is_run) {
				// start
				size_t threnad_num = _ttoi (m_qps_thread_num->GetText ().data ());
				if (threnad_num < 1) {
					m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _IT (_T ("The number of threads must be >= 1.")));
					return true;
				} else if (threnad_num > 100) {
					m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _IT (_T ("The number of threads must be <= 100.")));
					return true;
				}
				m_desp = _T ("");
				m_succ_counts.resize (threnad_num);
				m_fail_counts.resize (threnad_num);
				m_last_succ_count = m_last_fail_count = 0;
				faw::string_t type = m_qps_type->GetText ();
				faw::string_t url = m_qps_url->GetText ();
				if (url.empty ())
					url = m_qps_url->GetTipValue ();
				if (m_qps_postdata_tab->GetCurSel () == 0)
					_convert_data (true);
				faw::string_t post_data = (type == _T ("POST") ? m_qps_postdata_src->GetText () : _T (""));
				std::tuple<faw::string_t, faw::string_t, faw::string_t> params = { type, url, post_data };
				m_is_run = !m_is_run;
				for (size_t i = 0; i < threnad_num; ++i) {
					m_succ_counts[i] = m_fail_counts[i] = 0;
					std::thread (std::bind (&page_Qps::thread_func, this, i, params)).detach ();
				}
				std::this_thread::sleep_for (std::chrono::milliseconds (100));
			} else {
				m_is_run = !m_is_run;
			}
			m_qps_begin->SetVisible (!m_is_run);
			m_qps_stop->SetVisible (m_is_run);
			return true;
		} else if (name == _T ("qps_change")) {
			bool is_show_list = (m_qps_postdata_tab->GetCurSel () == 0);
			_convert_data (is_show_list);
			m_qps_postdata_tab->SelectItem (is_show_list ? 1 : 0);
			return true;
		} else if (name == _T ("qps_postdata_ctrl_up")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_qps_postdata_list->GetItemIndex (container);
			if (n == 0)
				return true;
			faw::string_t name1 { container->GetName () };
			faw::string_t name2 { m_qps_postdata_list->GetItemAt (n - 1)->GetName () };
			_swap_item (name1, name2);
			return true;
		} else if (name == _T ("qps_postdata_ctrl_down")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_qps_postdata_list->GetItemIndex (container);
			if (n >= m_qps_postdata_list->GetCount () - 2)
				return true;
			faw::string_t name1 { container->GetName () };
			faw::string_t name2 { m_qps_postdata_list->GetItemAt (n + 1)->GetName () };
			_swap_item (name1, name2);
			return true;
		} else if (name == _T ("qps_postdata_ctrl_new")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			m_qps_postdata_list->Remove (container);
			_add_item (_T (""), _T (""), _IT (_T ("Delete")));
			_add_item (_T (""), _T (""), _IT (_T ("Create")));
			return true;
		} else if (name == _T ("qps_postdata_ctrl_del")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_qps_postdata_list->GetItemIndex (container);
			m_qps_postdata_list->Remove (container);
			for (; n < m_qps_postdata_list->GetCount (); ++n) {
				container = m_qps_postdata_list->GetItemAt (n);
				faw::string_t _name { container->GetName () };
				faw::string_t color = (n % 2) ? _T ("#FFDDDDDD") : _T ("#FFFFFFFF");
				faw::string_t _name1 = _name + _T ("_key");
				BindEditUI edit_key { _name1 };
				edit_key->SetAttribute (_T ("bkcolor"), color);
				edit_key->SetAttribute (_T ("nativebkcolor"), color);
				_name1 = _name + _T ("_value");
				BindEditUI edit_value { _name1 };
				edit_value->SetAttribute (_T ("bkcolor"), color);
				edit_value->SetAttribute (_T ("nativebkcolor"), color);
			}
			return true;
		}
		return false;
	}

	bool OnItemSelect (TNotifyUI& msg) override {
		 faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("qps_type")) {
			bool is_post = m_qps_type->GetText () == _T ("POST");
			m_qps_change->SetEnabled (is_post);
			m_qps_postdata_src->SetEnabled (is_post);
			m_qps_postdata_src->SetTextColor (is_post ? 0xFF000000 : 0xFF777777);
			if (!is_post) {
				if (m_qps_postdata_tab->GetCurSel () == 0) {
					_convert_data (true);
					m_qps_postdata_tab->SelectItem (1);
				}
			}
			return true;
		}
		return false;
	}

protected:
	void thread_func (size_t n, std::tuple<faw::string_t, faw::string_t, faw::string_t> params) {
		auto[wtype, wurl, wpost_data] = params;
		auto _url = faw::Encoding::T_to_gb18030 (wurl);
		auto _post_data = faw::Encoding::T_to_gb18030 (wpost_data);
		while (m_is_run) {
			try {
				if (wtype == _T ("POST")) {
					tool_WebRequest::post (_url, _post_data);
				} else {
					tool_WebRequest::get (_url);
				}
				m_succ_counts[n]++;
			} catch (...) {
				m_fail_counts[n]++;
			}
		}
	}

	// 新增一行
	void _add_item (faw::string_t key, faw::string_t value, faw::string_t btntext) {
		static size_t n_sign = 0;
		CListContainerElementUI *item = new CListContainerElementUI ();
		item->SetFixedHeight (20);
		faw::string_t color = (m_qps_postdata_list->GetCount () % 2) ? _T ("#FFEEEEEE") : _T ("#FFFFFFFF");
		bool is_new = (btntext == _IT (_T ("Create")));
		if (is_new)
			color = _T ("#FFDDDDDD");
		item->SetAttribute (_T ("name"), fmt::format (_T ("qps_postdata_item_{}"), ++n_sign));
		//
		CContainerUI *ctnr = new CContainerUI ();
		CControlUI *ctrl = new CEditUI ();
		ctrl->SetManager (m_parent->get_pm (), item);
		ctrl->SetText (key);
		ctrl->SetAttribute (_T ("name"), fmt::format (_T ("qps_postdata_item_{}_key"), n_sign));
		ctrl->SetAttribute (_T ("bkcolor"), color);
		ctrl->SetAttribute (_T ("nativebkcolor"), color);
		ctrl->SetAttribute (_T ("align"), _T ("center"));
		ctrl->SetAttribute (_T ("padding"), _T ("10,4,0,4"));
		if (is_new)
			ctrl->SetAttribute (_T ("enabled"), _T ("false"));
		ctnr->Add (ctrl);
		item->Add (ctnr);
		//
		ctnr = new CContainerUI ();
		ctrl = new CEditUI ();
		ctrl->SetManager (m_parent->get_pm (), item);
		ctrl->SetText (value);
		ctrl->SetAttribute (_T ("name"), fmt::format (_T ("qps_postdata_item_{}_value"), n_sign));
		ctrl->SetAttribute (_T ("bkcolor"), color);
		ctrl->SetAttribute (_T ("nativebkcolor"), color);
		ctrl->SetAttribute (_T ("align"), _T ("center"));
		ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
		if (is_new)
			ctrl->SetAttribute (_T ("enabled"), _T ("false"));
		ctnr->Add (ctrl);
		item->Add (ctnr);
		//
		ctnr = new CHorizontalLayoutUI ();
		ctrl = new CButtonUI ();
		ctrl->SetManager (m_parent->get_pm (), ctnr);
		ctrl->SetText (_T ("↑"));
		ctrl->SetAttribute (_T ("style"), _T ("x_button"));
		ctrl->SetAttribute (_T ("name"), _T ("qps_postdata_ctrl_up"));
		ctrl->SetAttribute (_T ("width"), _T ("20"));
		if (is_new)
			ctrl->SetAttribute (_T ("visible"), _T ("false"));
		ctnr->Add (ctrl);
		ctrl = new CButtonUI ();
		ctrl->SetManager (m_parent->get_pm (), ctnr);
		ctrl->SetText (_T ("↓"));
		ctrl->SetAttribute (_T ("style"), _T ("x_button"));
		ctrl->SetAttribute (_T ("name"), _T ("qps_postdata_ctrl_down"));
		ctrl->SetAttribute (_T ("width"), _T ("20"));
		if (is_new)
			ctrl->SetAttribute (_T ("visible"), _T ("false"));
		ctnr->Add (ctrl);
		ctrl = new CButtonUI ();
		ctrl->SetManager (m_parent->get_pm (), ctnr);
		ctrl->SetText (btntext);
		ctrl->SetAttribute (_T ("style"), _T ("x_button"));
		ctrl->SetAttribute (_T ("name"), (is_new ? _T ("qps_postdata_ctrl_new") : _T ("qps_postdata_ctrl_del")));
		ctrl->SetAttribute (_T ("width"), (is_new ? _T ("80") : _T ("40")));
		ctnr->Add (ctrl);
		item->Add (ctnr);
		m_qps_postdata_list->Add (item);
	}

	// 交换两行内容
	void _swap_item (faw::string_t name1, faw::string_t name2) {
		faw::string_t name_key1 = name1 + _T ("_key");
		BindEditUI edit_key1 { name_key1 };
		faw::string_t name_key2 = name2 + _T ("_key");
		BindEditUI edit_key2 { name_key2 };
		faw::string_t name_tmp = edit_key1->GetText ();
		edit_key1->SetText (edit_key2->GetText ());
		edit_key2->SetText (name_tmp);
		//
		faw::string_t name_value1 = name1 + _T ("_value");
		BindEditUI edit_value1 { name_value1 };
		faw::string_t name_value2 = name2 + _T ("_value");
		BindEditUI edit_value2 { name_value2 };
		name_tmp = edit_value1->GetText ();
		edit_value1->SetText (edit_value2->GetText ());
		edit_value2->SetText (name_tmp);
	}

	// 表格数据与原始数据互转
	void _convert_data (bool list2src) {
		if (list2src) {
			faw::string_t post_data = _T ("");
			for (int i = 0; i < m_qps_postdata_list->GetCount () - 1; ++i) {
				CControlUI *ctrl = m_qps_postdata_list->GetItemAt (i);
				faw::string_t name { ctrl->GetName () };
				faw::string_t name_key = name + _T ("_key");
				BindEditUI edit_key { name_key };
				faw::string_t name_value = name + _T ("_value");
				BindEditUI edit_value { name_value };
				if (!post_data.empty ())
					post_data += _T ("&");
				post_data += faw::Encoding::percent_str_encode (edit_key->GetText ());
				post_data += _T ("=");
				post_data += faw::Encoding::percent_str_encode (edit_value->GetText ());
			}
			m_qps_postdata_src->SetText (post_data);
		} else {
			faw::string_t post_data = m_qps_postdata_src->GetText ();
			std::vector<faw::string_t> vitems = tool_StringT::split (post_data, _T ('&'), _T ("&amp;"), true);
			m_qps_postdata_list->RemoveAll ();
			for (size_t i = 0; i < vitems.size (); ++i) {
				std::vector<faw::string_t> vkey_val;
				tool_StringT::split (vitems [i], vkey_val, _T ('='));
				while (vkey_val.size () < 2)
					vkey_val.push_back (_T (""));
				vkey_val[0] = faw::Encoding::percent_str_decode (vkey_val[0]);
				vkey_val[1] = faw::Encoding::percent_str_decode (vkey_val[1]);
				_add_item (vkey_val[0], vkey_val[1], _IT (_T ("Delete")));
			}
			_add_item (_T (""), _T (""), _IT (_T ("Create")));
		}
	}

	BindComboUI				m_qps_type { _T ("qps_type") };
	BindEditUI				m_qps_thread_num { _T ("qps_thread_num") };
	BindEditUI				m_qps_url { _T ("qps_url") };
	BindButtonUI			m_qps_begin { _T ("qps_begin") };
	BindButtonUI			m_qps_stop { _T ("qps_stop") };
	BindButtonUI			m_qps_change { _T ("qps_change") };
	BindTabLayoutUI			m_qps_postdata_tab { _T ("qps_postdata_tab") };
	BindListUI				m_qps_postdata_list { _T ("qps_postdata_list") };
	BindRichEditUI			m_qps_postdata_src { _T ("qps_postdata_src") };
	BindRichEditUI			m_qps_content { _T ("qps_content") };
	volatile bool			m_is_run				= false;
	faw::string_t				m_desp					= _T ("");
	std::vector<size_t>		m_succ_counts;
	std::vector<size_t>		m_fail_counts;
	size_t					m_last_succ_count		= 0;
	size_t					m_last_fail_count		= 0;
};

#endif //__PAGE_QPS_HPP__
