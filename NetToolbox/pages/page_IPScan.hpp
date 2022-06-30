#ifndef __PAGE_IPSCAN_HPP__
#define __PAGE_IPSCAN_HPP__

#include <chrono>
#include <thread>
#include <functional>
#include <asio.hpp>

#include "../tools/tool_NetInfo.hpp"
#include "../tools/tool_Utils.hpp"

#include "page_base.hpp"



class page_IPScan: public page_base {
public:
	page_IPScan (NetToolboxWnd *parent): page_base (parent) {
		auto[ip1, ip2] = tool_NetInfo::get_ip_segment ();
		m_ipscan_ip1->SetText (ip1);
		m_ipscan_ip2->SetText (ip2);
	}
	virtual ~page_IPScan () = default;

	bool OnClick (TNotifyUI& msg) override {
		 faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("ipscan_start")) {
			faw::string_t sip1 = m_ipscan_ip1->GetText (), sip2 = m_ipscan_ip2->GetText ();
			uint32_t ip1 = tool_Utils::from_ipv4_my (sip1), ip2 = tool_Utils::from_ipv4_my (sip2);
			if (ip1 > ip2) {
				::MessageBox (m_parent->GetHWND (), _IT (_T ("Start IP cannot be greater than stop IP!")).data (), _IT (_T ("Error")).data (), MB_ICONHAND);
			} else if (ip2 - ip1 > 65536) {
				::MessageBox (m_parent->GetHWND (), _IT (_T ("This function does not support very large range IP scan!")).data (), _IT (_T ("Error")).data (), MB_ICONHAND);
			} else {
				m_ipscan_start->SetEnabled (false);
				m_ipscan_list->RemoveAll ();
				std::thread (&page_IPScan::scan_thread, this, ip1, ip2).detach ();
			}
			return true;
		}
		return false;
	}

private:
	void scan_thread (uint32_t ip1, uint32_t ip2) {
		static uint8_t send_data[50] = {
			0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x20, 0x43, 0x4b, 0x41, 0x41, 0x41, 0x41, 0x41,
			0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
			0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
			0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x21, 0x00, 0x01
		};
		static uint8_t recv_data[512];
		asio::const_buffer send_buf { send_data, sizeof (send_data) };
		asio::io_service service;
		asio::ip::udp::socket sock (service);
		sock.open (asio::ip::udp::endpoint (asio::ip::udp::v4 (), 25252).protocol ());
		static asio::ip::udp::endpoint sender_ep;
		//
		std::function<void (asio::error_code, std::size_t)> on_receive;
		on_receive = [this, &sock, &on_receive] (asio::error_code ec, std::size_t length) {
			if (!ec && length > 0) {
				m_parent->async_invoke ([this] () -> LRESULT {
					CListContainerElementUI *item = new CListContainerElementUI ();
					item->SetFixedHeight (20);
					//
					CControlUI *ctrl = new CControlUI ();
					ctrl->SetBkImage (_T ("file='list_ico_1.png' source='0,0,16,16' dest='0,0,16,16'"));
					//ctrl->SetText (is_ipv4 ? _T ("TCP") : _T ("UDP"));
					ctrl->SetAttribute (_T ("align"), _T ("center"));
					ctrl->SetAttribute (_T ("padding"), _T ("7,2,7,2"));
					item->Add (ctrl);
					m_ipscan_list->Add (item);
					return 0;
				});
			}
			sock.async_receive_from (asio::buffer (recv_data, sizeof (recv_data)), sender_ep, [on_receive] (asio::error_code ec, std::size_t length) { on_receive (ec, length); });
		};
		//on_receive (system::error_code (), 0);
		//
		while (ip1 <= ip2) {
			std::string sip = faw::Encoding::T_to_gb18030 (tool_Utils::format_ipv4_my (ip1));
			asio::ip::udp::endpoint ep { asio::ip::address::from_string (sip), 137 };
			sock.send_to (send_buf, ep);
			++ip1;
			service.run_one ();
		}
		std::chrono::system_clock::time_point tp = std::chrono::system_clock::now ();
		while (std::chrono::system_clock::now () - tp < std::chrono::seconds (1)) {
			service.run_one ();
			std::this_thread::sleep_for (std::chrono::milliseconds (1));
		}
		m_parent->invoke ([this] () -> LRESULT {
			m_ipscan_start->SetEnabled (true);
			return 0;
		});
	}

	BindEditUI				m_ipscan_ip1 { _T ("ipscan_ip1") };
	BindEditUI				m_ipscan_ip2 { _T ("ipscan_ip2") };
	BindButtonUI			m_ipscan_start { _T ("ipscan_start") };
	BindListUI				m_ipscan_list { _T ("ipscan_list") };
};

#endif //__PAGE_IPSCAN_HPP__
