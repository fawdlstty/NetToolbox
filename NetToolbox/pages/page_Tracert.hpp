#ifndef __PAGE_TRACERT_HPP__
#define __PAGE_TRACERT_HPP__

#include <thread>
#include <memory>

#include "page_base.hpp"
#include "../tools/tool_Tracert.hpp"
#include "../tools/tool_Tracert2.hpp"
#include "../tools/tool_DnsLookup.hpp"
#include "../tools/tool_Formatting.hpp"
#include "../tools/tool_String.hpp"
#include "../tools/tool_Resource.hpp"
#include "../tools/tool_QQWry.hpp"

#define INDEX_BLOCK_LENGTH 12



class page_Tracert: public page_base {
	//uint8_t *m_ipdata = nullptr;

public:
	page_Tracert (NetToolboxWnd *parent)
		: page_base (parent)
		, m_qqwry (tool_QQWry (tool_Resource::load_binary ((HMODULE) CPaintManagerUI::GetInstance (), _T ("IPDB"), MAKEINTRESOURCE (IDR_IPDB1)))) {}

	bool OnClick (TNotifyUI& msg) override {
		faw::String name = msg.pSender->GetName ();
		if (name == _T ("tracert_begin")) {
			if (!m_tracert.is_running ()) {
				 faw::String addr = m_tracert_addr->GetText ();
				if (addr.empty ())
					addr = m_tracert_addr->GetTipValue ();
				size_t p;
				if ((p = addr.find (_T ("//"))) != faw::String::_npos)
					addr = addr.substr (p + 2);
				if ((p = addr.find (_T ("/"))) != faw::String::_npos)
					addr = addr.substr (0, p);
				if ((p = addr.find (_T (":"))) != faw::String::_npos)
					addr = addr.substr (0, p);
				std::string s = addr.stra ();
				if (tool_Formatting::is_domain (s)) {
					s = tool_DnsLookup::query_ip (s.c_str ());
				}
				addr = s;
				bool is_ipv4 = tool_Formatting::is_ipv4 (s), is_ipv6 = tool_Formatting::is_ipv6 (s);
				if (!is_ipv4 /*    TODO    && !is_ipv6*/) {
					m_parent->show_status (NetToolboxWnd::StatusIcon::Error, International::translate (_T ("Unknown IP or domain name")));
				} else {
					m_tracert_begin->SetText ("停止");
					m_tracert_list->RemoveAll ();
					m_parent->show_status (NetToolboxWnd::StatusIcon::Loading, faw::String::format (International::translate (_T ("Routing trace to %s")).data (), addr.c_str ()));
					m_tracert.set_abort_callback ([this] (faw::String _err_msg) {
						m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _err_msg);
						// TODO 恢复为未跟踪状态
						m_tracert_begin->SetText ("重新开始");
					});
					m_tracert.set_view_callback ([this] (std::vector<Tracert2ViewItem> _views) {
						while (m_tracert_list->GetCount () > _views.size ()) {
							m_tracert_list->RemoveAt (_views.size ());
						}
						while (m_tracert_list->GetCount () < _views.size ()) {
							auto item = new CListContainerElementUI ();
							item->SetFixedHeight (20);
							CControlUI* ctrl = new CContainerUI ();
							ctrl->SetBkImage (_T ("file='list_ico_2.png' source='0,0,16,16' dest='0,0,16,16'"));
							ctrl->SetAttribute (_T ("padding"), _T ("7,2,7,2"));
							item->Add (ctrl);
							for (size_t i = 0; i < 6; ++i) {
								ctrl = new CTextUI ();
								ctrl->SetAttribute (_T ("align"), _T ("center"));
								ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
								item->Add (ctrl);
							}
							ctrl = new CTextUI ();
							ctrl->SetAttribute (_T ("align"), _T ("left"));
							ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
							item->Add (ctrl);
							m_tracert_list->Add (item);
						}
						for (size_t i = 0; i < _views.size (); ++i) {
							auto item = dynamic_cast<CListContainerElementUI*> (m_tracert_list->GetItemAt (i));
							dynamic_cast<CTextUI*> (item->GetItemAt (1))->SetText (_views [i].m_ip);
							dynamic_cast<CTextUI*> (item->GetItemAt (2))->SetText (_views [i].m_loss);
							dynamic_cast<CTextUI*> (item->GetItemAt (3))->SetText (_views [i].m_sent_n);
							dynamic_cast<CTextUI*> (item->GetItemAt (4))->SetText (_views [i].m_recv_n);
							dynamic_cast<CTextUI*> (item->GetItemAt (5))->SetText (_views [i].m_best);
							dynamic_cast<CTextUI*> (item->GetItemAt (6))->SetText (_views [i].m_worst);
							auto [_a, _b] = m_qqwry.find_info (_views [i].m_ip);
							dynamic_cast<CTextUI*> (item->GetItemAt (7))->SetText (_a);
						}
					});
					m_tracert.start_ipv4 (s);
				}
			} else {
				m_tracert.stop ();
			}
			return true;
		}
		return false;
	}

protected:
	//void on_show_info (size_t row, size_t col, std::string data) {
	//	faw::String _data = faw::Encoding::gb18030_to_T (data);
	//	m_parent->invoke ([&] () -> LRESULT {
	//		CListContainerElementUI *item = nullptr;
	//		if ((size_t) m_tracert_list->GetCount () <= row - 1) {
	//			item = new CListContainerElementUI ();
	//			item->SetFixedHeight (20);
	//			//
	//			CControlUI *ctrl = new CContainerUI ();
	//			ctrl->SetBkImage (_T ("file='list_ico_2.png' source='0,0,16,16' dest='0,0,16,16'"));
	//			ctrl->SetAttribute (_T ("padding"), _T ("7,2,7,2"));
	//			item->Add (ctrl);
	//			for (size_t i = 0; i < 5; ++i) {
	//				ctrl = new CTextUI ();
	//				ctrl->SetAttribute (_T ("align"), _T ("center"));
	//				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
	//				item->Add (ctrl);
	//			}
	//			ctrl = new CTextUI ();
	//			ctrl->SetAttribute (_T ("align"), _T ("left"));
	//			ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
	//			item->Add (ctrl);
	//			m_tracert_list->Add (item);
	//		} else {
	//			item = dynamic_cast<CListContainerElementUI*> (m_tracert_list->GetItemAt ((int) row - 1));
	//			if (col == 4) {
	//				if (data != "" && data != "0.0.0.0") {
	//					auto [_a, _b] = m_qqwry.find_info (data);
	//					dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 2))->SetText (_a);
	//				}
	//			}
	//		}
	//		dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 1))->SetText (_data.c_str ());
	//		return 0;
	//	});
	//}

protected:
	BindButtonUI	m_tracert_begin { _T ("tracert_begin") };
	BindEditUI		m_tracert_addr { _T ("tracert_addr") };
	BindListUI		m_tracert_list { _T ("tracert_list") };

	tool_QQWry		m_qqwry;
	tool_Tracert2	m_tracert;
};

#endif //__PAGE_TRACERT_HPP__
