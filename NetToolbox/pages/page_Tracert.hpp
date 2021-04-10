#ifndef __PAGE_TRACERT_HPP__
#define __PAGE_TRACERT_HPP__

#include <thread>
#include <memory>

#include "page_base.hpp"
#include "../tools/tool_Tracert.hpp"
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
		faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("tracert_begin")) {
			m_tracert_begin->SetEnabled (false);
			faw::string_t addr = m_tracert_addr->GetText ();
			if (addr.empty ())
				addr = m_tracert_addr->GetTipValue ();
			size_t p;
			if ((p = addr.find (_T ("//"))) != faw::string_t::npos)
				addr = addr.substr (p + 2);
			if ((p = addr.find (_T ("/"))) != faw::string_t::npos)
				addr = addr.substr (0, p);
			if ((p = addr.find (_T (":"))) != faw::string_t::npos)
				addr = addr.substr (0, p);
			if (tool_Formatting::is_domain (addr)) {
				addr = tool_DnsLookup::query_ip (addr);
			}
			bool is_ipv4 = tool_Formatting::is_ipv4 (addr), is_ipv6 = tool_Formatting::is_ipv6 (addr);
			if (!is_ipv4 && !is_ipv6) {
				m_parent->show_status (NetToolboxWnd::StatusIcon::Error, International::translate (_T ("Unknown IP or domain name")));
				m_tracert_begin->SetEnabled (true);
			} else {
				m_tracert_list->RemoveAll ();
				m_parent->show_status (NetToolboxWnd::StatusIcon::Loading, fmt::format (International::translate (_T ("Routing trace to {}")), addr));
				std::thread ([this, addr, is_ipv4] () {
					bool _state;
					faw::string_t _info;
					auto f = std::bind (&page_Tracert::on_show_info, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
					std::string _addr = faw::Encoding::T_to_gb18030 (addr);
					if (is_ipv4) {
						std::tie (_state, _info) = tool_Tracert::start_ipv4 (_addr, f);
					} else {
						std::tie (_state, _info) = tool_Tracert::start_ipv6 (_addr, f);
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
		faw::string_t _data = faw::Encoding::gb18030_to_T (data);
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
				ctrl = new CTextUI ();
				ctrl->SetAttribute (_T ("align"), _T ("left"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				m_tracert_list->Add (item);
			} else {
				item = dynamic_cast<CListContainerElementUI*> (m_tracert_list->GetItemAt ((int) row - 1));
				if (col == 4) {
					if (data != "" && data != "0.0.0.0") {
						auto [_a, _b, _c] = m_qqwry.find_info (data);
						std::string _ab = fmt::format ("{} {} {}", _a, _b, _c);
						dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 2))->SetText (faw::Encoding::gb18030_to_T (_ab));
					}
				}
			}
			dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 1))->SetText (_data.data ());
			return 0;
		});
	}

protected:
	BindButtonUI	m_tracert_begin { _T ("tracert_begin") };
	BindEditUI		m_tracert_addr { _T ("tracert_addr") };
	BindListUI		m_tracert_list { _T ("tracert_list") };

	tool_QQWry		m_qqwry;
};

#endif //__PAGE_TRACERT_HPP__
