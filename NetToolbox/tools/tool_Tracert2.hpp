////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Tracert2
// Description: 路由跟踪工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_TRACERT2_HPP__
#define __TOOL_TRACERT2_HPP__

#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <functional>
#include <cstdint>
#include <Windows.h>
#include <IcmpAPI.h>
#include <iphlpapi.h>
#include <ip2string.h>

#include "tool_String.hpp"
#include "tool_Utils.hpp"
#include "tool_ObjectPool.hpp"
#include "../../DuiLib/3rd/FawLib/FawLib.hpp"
#include "../International.hpp"

#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")



struct Tracert2ViewItem {
	std::string m_ip;
	std::string m_loss;
	int32_t m_sent_count;
	int32_t m_recv_count;
	std::string m_best;
	std::string m_worst;
	std::string m_last;
};

struct Tracert2Ipv4SentItem {
	HANDLE m_icmp = INVALID_HANDLE_VALUE;
	uint8_t m_sent_buf [32];
	uint8_t m_recv_buf [sizeof (ICMP_ECHO_REPLY) + sizeof (m_sent_buf)];
	PICMP_ECHO_REPLY m_recv_ptr;
	IP_OPTION_INFORMATION m_ioi = { 0, 0, 0, 0, nullptr };

	Tracert2Ipv4SentItem () {
		memset (m_sent_buf, 0x65, sizeof (m_sent_buf));
		memset (m_recv_buf, 0x65, sizeof (m_recv_buf));
		m_recv_ptr = (PICMP_ECHO_REPLY) m_recv_buf;
	}
	~Tracert2Ipv4SentItem () {
		if (m_icmp != INVALID_HANDLE_VALUE) {
			::IcmpCloseHandle (m_icmp);
			m_icmp = INVALID_HANDLE_VALUE;
		}
	}

	std::tuple<bool, faw::String> init (uint8_t _ttl) {
		m_ioi.Ttl = _ttl;
		if (m_icmp == INVALID_HANDLE_VALUE) {
			m_icmp = ::IcmpCreateFile ();
			if (m_icmp == INVALID_HANDLE_VALUE)
				return { false, faw::String::format (_T ("IcmpCreateFile %s"), tool_Utils::get_error_info (::GetLastError ()).c_str ()) };
		}
		//
		return { true, _T ("") };
	}

	void do_send (std::function<void (Tracert2RecvItem)> _recv_callback) {
		//
	}
};

struct Tracert2RecvItem {
	std::string m_ip;
	int32_t m_elapse;
};

struct Tracert2CountingItem {
	std::string m_ip;
	std::vector<int32_t> m_elapses;
};

class tool_Tracert2 {
public:
	tool_Tracert2 () {}
	~tool_Tracert2 () { stop (); }

	void set_view_callback (std::function<void (std::unique_ptr<std::vector<Tracert2ViewItem>>)> _set_view) { m_set_view = _set_view; }
	void set_abort_callback (std::function<void (faw::String)> _abort) { m_abort = _abort; }

	// 开始ipv4路由跟踪
	std::tuple<bool, faw::String> start_ipv4 (std::string dest_ip) {
		m_thread = std::move (std::make_unique<std::thread> ([this] () {
			
		}));
	}

	void stop () {
		if (m_thread != nullptr && m_thread->joinable ()) {
			m_thread->join ();
			m_thread = nullptr;
		}
	}

	//// 开始ipv4路由跟踪
	//static std::tuple<bool, faw::String> start_ipv4 (std::string dest_ip, std::function<void (size_t, size_t, std::string)> f) {
	//	IPAddr ul_dest_ip = ::inet_addr (&dest_ip [0]);
	//	HANDLE hIcmp = ::IcmpCreateFile ();
	//	if (hIcmp == INVALID_HANDLE_VALUE)
	//		return { false, faw::String::format (_T ("IcmpCreateFile %s"), tool_Utils::get_error_info (::GetLastError ()).c_str ()) };
	//	constexpr DWORD reply_size = sizeof (ICMP_ECHO_REPLY) + sizeof (m_send);
	//	BYTE b_reply [reply_size];
	//	PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY) b_reply;
	//	IP_OPTION_INFORMATION ioi = { 0, 0, 0, 0, nullptr };
	//	for (UCHAR ttl = 1; ttl <= m_max_ttl; ++ttl) {
	//		ioi.Ttl = ttl;
	//		f (ttl, 0, tool_StringA::format ("%d", ttl));
	//		std::string route_ip = "0.0.0.0";
	//		for (size_t i = 1; i <= m_test_count; ++i) {
	//			DWORD dwRetVal = ::IcmpSendEcho (hIcmp, ul_dest_ip, m_send, sizeof (m_send), &ioi, reply, reply_size, 1000);
	//			if (reply->Status == IP_TTL_EXPIRED_TRANSIT || reply->Status == IP_TTL_EXPIRED_REASSEM || reply->Status == IP_SUCCESS) {
	//				if (reply->RoundTripTime == 0) {
	//					f (ttl, i, "<1ms");
	//				} else {
	//					f (ttl, i, tool_StringA::format ("%dms", reply->RoundTripTime));
	//				}
	//			} else if (reply->Status == IP_REQ_TIMED_OUT) {
	//				f (ttl, i, "*");
	//			} else {
	//				f (ttl, i, "?");
	//			}
	//			if (route_ip == "0.0.0.0") {
	//				in_addr _addr { 0 };
	//				_addr.s_addr = reply->Address;
	//				route_ip = ::inet_ntoa (_addr);
	//			}
	//		}
	//		f (ttl, 4, route_ip);
	//		if (dest_ip == route_ip) {
	//			::IcmpCloseHandle (hIcmp);
	//			return { true, International::translate (_T ("Tracert Complete.")) };
	//		}
	//	}
	//	::IcmpCloseHandle (hIcmp);
	//	return { true, International::translate (_T ("Routing trace has ended and the maximum traceable hop count has been exceeded.")) };
	//}

private:
	//static size_t	m_test_count;
	//static UCHAR	m_max_ttl;
	//static BYTE		m_send [32];
	std::function<void (std::unique_ptr<std::vector<Tracert2ViewItem>>)> m_set_view;
	std::unique_ptr<std::thread> m_thread;
	std::function<void (faw::String)> m_abort;

	// tmp
	tool_ObjectPool<std::vector<Tracert2ViewItem>> m_views_pool;
};

//inline size_t		tool_Tracert::m_test_count = 3;
//inline UCHAR		tool_Tracert::m_max_ttl = 30;
//inline BYTE			tool_Tracert::m_send [32] { 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65 };

#endif //__TOOL_TRACERT2_HPP__
