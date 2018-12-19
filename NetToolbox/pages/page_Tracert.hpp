#ifndef __PAGE_TRACERT_HPP__
#define __PAGE_TRACERT_HPP__

#include <thread>
#include <memory>

#include "page_base.hpp"
#include "../tools/tool_Tracert.hpp"
#include "../tools/tool_DnsLookup.hpp"
#include "../tools/tool_Formatting.hpp"
#include "../tools/tool_Encoding.hpp"
#include "../tools/tool_String.hpp"



class page_Tracert: public page_base {
public:
	page_Tracert (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_Tracert () = default;

	bool OnClick (TNotifyUI& msg) override {
		CDuiString name = msg.pSender->GetName ();
		if (name == _T ("tracert_begin")) {
			m_tracert_begin->SetEnabled (false);
			CDuiString addr = m_tracert_addr->GetText ();
			if (addr.empty ())
				addr = m_tracert_addr->GetTipValue ();
			size_t p;
			if ((p = addr.find (_T ("//"))) != string_t::npos)
				addr = addr.substr (p + 2);
			if ((p = addr.find (_T ("/"))) != string_t::npos)
				addr = addr.substr (0, p);
			if ((p = addr.find (_T (":"))) != string_t::npos)
				addr = addr.substr (0, p);
			std::string s = tool_Encoding::get_gb18030 (addr);
			if (tool_Formatting::is_domain (s)) {
				s = tool_DnsLookup::query_ip (s.c_str ());
			}
			addr = tool_Encoding::get_T (s);
			bool is_ipv4 = tool_Formatting::is_ipv4 (s), is_ipv6 = tool_Formatting::is_ipv6 (s);
			if (!is_ipv4 && !is_ipv6) {
				m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _T ("未知地址或域名"));
				m_tracert_begin->SetEnabled (true);
			} else {
				m_tracert_list->RemoveAll ();
				m_parent->show_status (NetToolboxWnd::StatusIcon::Loading, tool_StringT::format (_T ("正在路由跟踪到 %s"), addr.c_str ()));
				std::thread ([this, s, is_ipv4] () {
					bool _state;
					string_t _info;
					auto f = std::bind (&page_Tracert::on_show_info, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
					if (is_ipv4) {
						std::tie (_state, _info) = tool_Tracert::start_ipv4 (s, f);
					} else {
						std::tie (_state, _info) = tool_Tracert::start_ipv6 (s, f);
					}
					this->m_parent->invoke ([&] () -> LRESULT {
						m_parent->show_status (_state ? NetToolboxWnd::StatusIcon::Ok : NetToolboxWnd::StatusIcon::Error, _info.c_str ());
						m_tracert_begin->SetEnabled (true);
						return 0;
					});
				}).detach ();
			}
			return true;
		}
		return false;
	}

protected:
	void on_show_info (size_t row, size_t col, std::string data) {
		string_t _data = tool_Encoding::get_T (data);
		m_parent->invoke ([&] () -> LRESULT {
			CListContainerElementUI *item = nullptr;
			if ((size_t) m_tracert_list->GetCount () <= row - 1) {
				item = new CListContainerElementUI ();
				item->SetFixedHeight (20);
				//
				CControlUI *ctrl = new CContainerUI ();
				ctrl->SetBkImage (_T ("file='list_ico_2.png' source='0,0,16,16' dest='0,0,16,16'"));
				ctrl->SetAttribute (_T ("padding"), _T ("7,2,7,2"));
				item->Add (ctrl);
				for (size_t i = 0; i < 5; ++i) {
					ctrl = new CTextUI ();
					ctrl->SetAttribute (_T ("align"), _T ("center"));
					ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
					item->Add (ctrl);
				}
				m_tracert_list->Add (item);
			} else {
				item = dynamic_cast<CListContainerElementUI*> (m_tracert_list->GetItemAt ((int) row - 1));
			}
			dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 1))->SetText (_data.c_str ());
			return 0;
		});
	}

protected:
	BindButtonUI	m_tracert_begin { _T ("tracert_begin") };
	BindEditUI		m_tracert_addr { _T ("tracert_addr") };
	BindListUI		m_tracert_list { _T ("tracert_list") };
};

#endif //__PAGE_TRACERT_HPP__
