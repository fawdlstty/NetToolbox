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
#include "../tools/tool_Encoding.hpp"
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
			m_desp += tool_StringT::format (_T ("发送 %-6d，成功 %-6d，失败 %d\n"), inc_succ_count + inc_fail_count, inc_succ_count, inc_fail_count);
			m_qps_content->SetText (m_desp.c_str ());
			m_last_succ_count += inc_succ_count;
			m_last_fail_count += inc_fail_count;
		}
	}

	bool OnClick (TNotifyUI& msg) override {
		CDuiString name = msg.pSender->GetName ();
		if (name == _T ("qps_begin") || name == _T ("qps_stop")) {
			if ((name == _T ("qps_begin")) == m_is_run)
				return true;
			if (!m_is_run) {
				// start
				size_t threnad_num = _ttoi (m_qps_thread_num->GetText ().c_str ());
				if (threnad_num < 1) {
					m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _T ("线程数至少为1。"));
					return true;
				} else if (threnad_num > 100) {
					m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _T ("线程数至多为100。"));
					return true;
				}
				m_desp = _T ("");
				m_succ_counts.resize (threnad_num);
				m_fail_counts.resize (threnad_num);
				m_last_succ_count = m_last_fail_count = 0;
				std::string type = tool_Encoding::get_gb18030 (m_qps_type->GetText ());
				std::string url = tool_Encoding::get_gb18030 (m_qps_url->GetText ());
				if (url.empty ())
					url = tool_Encoding::get_utf8 (m_qps_url->GetTipValue ());
				if (m_qps_postdata_tab->GetCurSel () == 0)
					_convert_data (true);
				std::string post_data = (type == "POST" ? tool_Encoding::get_utf8 (m_qps_postdata_src->GetText ()) : "");
				std::tuple<std::string, std::string, std::string> params = { type, url, post_data };
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
			string_t name1 { container->GetName () };
			string_t name2 { m_qps_postdata_list->GetItemAt (n - 1)->GetName () };
			_swap_item (name1, name2);
			return true;
		} else if (name == _T ("qps_postdata_ctrl_down")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_qps_postdata_list->GetItemIndex (container);
			if (n >= m_qps_postdata_list->GetCount () - 2)
				return true;
			string_t name1 { container->GetName () };
			string_t name2 { m_qps_postdata_list->GetItemAt (n + 1)->GetName () };
			_swap_item (name1, name2);
			return true;
		} else if (name == _T ("qps_postdata_ctrl_new")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			m_qps_postdata_list->Remove (container);
			_add_item (_T (""), _T (""), _T ("删除"));
			_add_item (_T (""), _T (""), _T ("新建"));
			return true;
		} else if (name == _T ("qps_postdata_ctrl_del")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_qps_postdata_list->GetItemIndex (container);
			m_qps_postdata_list->Remove (container);
			for (; n < m_qps_postdata_list->GetCount (); ++n) {
				container = m_qps_postdata_list->GetItemAt (n);
				string_t _name { container->GetName () };
				string_t color = (n % 2) ? _T ("#FFDDDDDD") : _T ("#FFFFFFFF");
				string_t _name1 = _name + _T ("_key");
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
		CDuiString name = msg.pSender->GetName ();
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
	void thread_func (size_t n, std::tuple<std::string, std::string, std::string> params) {
		auto[type, url, post_data] = params;
		while (m_is_run) {
			try {
				if (type == "POST") {
					tool_WebRequest::post (url, post_data);
				} else {
					tool_WebRequest::get (url);
				}
				m_succ_counts[n]++;
			} catch (...) {
				m_fail_counts[n]++;
			}
		}
	}

	// 新增一行
	void _add_item (string_view_t key, string_view_t value, string_view_t btntext) {
		static size_t n_sign = 0;
		CListContainerElementUI *item = new CListContainerElementUI ();
		item->SetFixedHeight (20);
		string_t color = (m_qps_postdata_list->GetCount () % 2) ? _T ("#FFEEEEEE") : _T ("#FFFFFFFF");
		bool is_new = (btntext == _T ("新建"));
		if (is_new)
			color = _T ("#FFDDDDDD");
		item->SetAttribute (_T ("name"), tool_StringT::format (_T ("qps_postdata_item_%d"), ++n_sign));
		//
		CContainerUI *ctnr = new CContainerUI ();
		CControlUI *ctrl = new CEditUI ();
		ctrl->SetManager (m_parent->get_pm (), item);
		ctrl->SetText (key);
		ctrl->SetAttribute (_T ("name"), tool_StringT::format (_T ("qps_postdata_item_%d_key"), n_sign));
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
		ctrl->SetAttribute (_T ("name"), tool_StringT::format (_T ("qps_postdata_item_%d_value"), n_sign));
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
	void _swap_item (string_t name1, string_t name2) {
		string_t name_key1 = name1 + _T ("_key");
		BindEditUI edit_key1 { name_key1 };
		string_t name_key2 = name2 + _T ("_key");
		BindEditUI edit_key2 { name_key2 };
		string_t name_tmp = edit_key1->GetText ();
		edit_key1->SetText (edit_key2->GetText ());
		edit_key2->SetText (name_tmp);
		//
		string_t name_value1 = name1 + _T ("_value");
		BindEditUI edit_value1 { name_value1 };
		string_t name_value2 = name2 + _T ("_value");
		BindEditUI edit_value2 { name_value2 };
		name_tmp = edit_value1->GetText ();
		edit_value1->SetText (edit_value2->GetText ());
		edit_value2->SetText (name_tmp);
	}

	// 表格数据与原始数据互转
	void _convert_data (bool list2src) {
		if (list2src) {
			string_t post_data = _T ("");
			for (int i = 0; i < m_qps_postdata_list->GetCount () - 1; ++i) {
				CControlUI *ctrl = m_qps_postdata_list->GetItemAt (i);
				string_t name { ctrl->GetName () };
				string_t name_key = name + _T ("_key");
				BindEditUI edit_key { name_key };
				string_t name_value = name + _T ("_value");
				BindEditUI edit_value { name_value };
				if (!post_data.empty ())
					post_data += _T ("&");
				post_data += tool_Encoding::get_T_from_utf8 (tool_Encoding::percent_str_encode (tool_Encoding::get_utf8 (edit_key->GetText ())));
				post_data += _T ("=");
				post_data += tool_Encoding::get_T_from_utf8 (tool_Encoding::percent_str_encode (tool_Encoding::get_utf8 (edit_value->GetText ())));
			}
			m_qps_postdata_src->SetText (post_data);
		} else {
			string_t post_data = m_qps_postdata_src->GetText ();
			std::vector<string_t> vitems = tool_StringT::split (post_data, _T ('&'), _T ("&amp;"), true);
			m_qps_postdata_list->RemoveAll ();
			for (size_t i = 0; i < vitems.size (); ++i) {
				std::vector<string_t> vkey_val = tool_StringT::split (vitems[i], _T ('='), _T (""), false);
				while (vkey_val.size () < 2)
					vkey_val.push_back (_T (""));
				vkey_val[0] = tool_Encoding::get_T_from_utf8 (tool_Encoding::percent_str_decode (tool_Encoding::get_utf8 (vkey_val[0])));
				vkey_val[1] = tool_Encoding::get_T_from_utf8 (tool_Encoding::percent_str_decode (tool_Encoding::get_utf8 (vkey_val[1])));
				_add_item (vkey_val[0], vkey_val[1], _T ("删除"));
			}
			_add_item (_T (""), _T (""), _T ("新建"));
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
	string_t				m_desp					= _T ("");
	std::vector<size_t>		m_succ_counts;
	std::vector<size_t>		m_fail_counts;
	size_t					m_last_succ_count		= 0;
	size_t					m_last_fail_count		= 0;
};

#endif //__PAGE_QPS_HPP__
