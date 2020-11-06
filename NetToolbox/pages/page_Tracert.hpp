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
		faw::String name = msg.pSender->GetName ();
		if (name == _T ("tracert_begin")) {
			m_tracert_begin->SetEnabled (false);
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
			if (!is_ipv4 && !is_ipv6) {
				m_parent->show_status (NetToolboxWnd::StatusIcon::Error, International::translate (_T ("Unknown IP or domain name")));
				m_tracert_begin->SetEnabled (true);
			} else {
				m_tracert_list->RemoveAll ();
				m_parent->show_status (NetToolboxWnd::StatusIcon::Loading, faw::String::format (International::translate (_T ("Routing trace to %s")).data (), addr.c_str ()));
				std::thread ([this, s, is_ipv4] () {
					bool _state;
					faw::String _info;
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
		faw::String _data = faw::Encoding::gb18030_to_T (data);
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
						auto [_a, _b] = m_qqwry.find_info (data);
						dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 2))->SetText (_a);
					}
				}
				//// search ip location
				//uint32_t ip = (uint32_t) ::ntohl (inet_addr (data.c_str ()));
				//if (col == 4) {
				//	if (_data != _T ("") && _data != _T ("0.0.0.0") && m_ipdata) {
				//		int32_t l = 0, h = m_total_blocks, m, p;
				//		uint32_t dptr = 0;
				//		uint8_t *_ipdata = m_ipdata;
				//		while (l <= h) {
				//			m = (l + h) >> 1;
				//			p = m_first_index_ptr + m * INDEX_BLOCK_LENGTH;
				//			_ipdata = m_ipdata + p;
				//			uint32_t sip = _get_unsigned_int (_ipdata, 0);
				//			if (ip < sip) {
				//				h = m - 1;
				//			} else {
				//				uint32_t eip = _get_unsigned_int (_ipdata, 4);
				//				if (ip > eip) {
				//					l = m + 1;
				//				} else {
				//					dptr = _get_unsigned_int (_ipdata, 8);
				//					break;
				//				}
				//			}
				//		}
				//		if (dptr != 0) {
				//			int32_t dataLen = ((dptr >> 24) & 0xFF);
				//			int32_t dataptr = (dptr & 0x00FFFFFF);
				//			_ipdata = m_ipdata + dataptr;
				//			uint32_t _city_id = _get_unsigned_int (_ipdata, 0);
				//			dataLen -= 4;
				//			static char s_region [MAX_PATH];
				//			memcpy (s_region, _ipdata + 4, dataLen);
				//			s_region [dataLen] = '\0';
				//			std::string _region_str = s_region;
				//			_region_str = faw::Encoding::utf8_to_gb18030 (_region_str);
				//			dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 2))->SetText (_region_str.c_str ());
				//		}
				//	}
				//}
			}
			dynamic_cast<CTextUI*> (item->GetItemAt ((int) col + 1))->SetText (_data.c_str ());
			return 0;
		});
	}

	//uint32_t _get_unsigned_int (uint8_t *_tmp_buff, int offset) {
	//	return (((_tmp_buff [offset]) & 0x000000FF)
	//		| ((_tmp_buff [offset + 1] << 8) & 0x0000FF00)
	//		| ((_tmp_buff [offset + 2] << 16) & 0x00FF0000)
	//		| ((_tmp_buff [offset + 3] << 24) & 0xFF000000));
	//}

protected:
	BindButtonUI	m_tracert_begin { _T ("tracert_begin") };
	BindEditUI		m_tracert_addr { _T ("tracert_addr") };
	BindListUI		m_tracert_list { _T ("tracert_list") };

	//uint32_t		m_first_index_ptr = 0, m_last_index_ptr = 0, m_total_blocks = 0;
	tool_QQWry		m_qqwry;
};

#endif //__PAGE_TRACERT_HPP__
