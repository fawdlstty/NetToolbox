////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Tracert
// Description: 路由跟踪工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_TRACERT_HPP__
#define __TOOL_TRACERT_HPP__

#include <string>
#include <tuple>
#include <functional>
#include <Windows.h>
#include <IcmpAPI.h>
#include <iphlpapi.h>
#include <ip2string.h>

#include "tool_String.hpp"
#include "tool_Utils.hpp"



class tool_Tracert {
public:
	// 开始ipv4路由跟踪
	static std::tuple<bool, string_t> start_ipv4 (std::string dest_ip, std::function<void (size_t, size_t, std::string)> f) {
		IPAddr ul_dest_ip = ::inet_addr (&dest_ip[0]);
		HANDLE hIcmp = ::IcmpCreateFile ();
		if (hIcmp == INVALID_HANDLE_VALUE)
			return { false, tool_StringT::format (_T ("IcmpCreateFile %s"), tool_Utils::get_error_info (::GetLastError ()).c_str ()) };
		constexpr DWORD reply_size = sizeof (ICMP_ECHO_REPLY) + sizeof (m_send);
		BYTE b_reply[reply_size];
		PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY) b_reply;
		IP_OPTION_INFORMATION ioi = { 0, 0, 0, 0, nullptr };
		for (UCHAR ttl = 1; ttl <= m_max_ttl; ++ttl) {
			ioi.Ttl = ttl;
			f (ttl, 0, tool_StringA::format ("%d", ttl));
			std::string route_ip = "0.0.0.0";
			for (size_t i = 1; i <= m_test_count; ++i) {
				DWORD dwRetVal = ::IcmpSendEcho (hIcmp, ul_dest_ip, m_send, sizeof (m_send), &ioi, reply, reply_size, 1000);
				if (reply->Status == IP_TTL_EXPIRED_TRANSIT || reply->Status == IP_TTL_EXPIRED_REASSEM || reply->Status == IP_SUCCESS) {
					if (reply->RoundTripTime == 0) {
						f (ttl, i, "<1ms");
					} else {
						f (ttl, i, tool_StringA::format ("%dms", reply->RoundTripTime));
					}
				} else if (reply->Status == IP_REQ_TIMED_OUT) {
					f (ttl, i, "*");
				} else {
					f (ttl, i, "?");
				}
				if (route_ip == "0.0.0.0") {
					in_addr _addr { 0 };
					_addr.s_addr = reply->Address;
					route_ip = ::inet_ntoa (_addr);
				}
			}
			f (ttl, 4, route_ip);
			if (dest_ip == route_ip) {
				::IcmpCloseHandle (hIcmp);
				return { true, _T ("路由跟踪完成。") };
			}
		}
		::IcmpCloseHandle (hIcmp);
		return { true, _T ("路由跟踪结束，已超过可跟踪的最大跃点数。") };
	}

	// 开始ipv6路由跟踪
	static std::tuple<bool, string_t> start_ipv6 (std::string dest_ip, std::function<void (size_t, size_t, std::string)> f) {
		in_addr6 ul_dest_ip = { 0 };
		::inet_pton (AF_INET6, dest_ip.c_str (), &ul_dest_ip);
		HANDLE hIcmp = ::Icmp6CreateFile ();
		if (hIcmp == INVALID_HANDLE_VALUE)
			return { false, tool_StringT::format (_T ("Icmp6CreateFile %s"), tool_Utils::get_error_info (::GetLastError ()).c_str ()) };
		constexpr DWORD reply_size = sizeof (ICMPV6_ECHO_REPLY) + sizeof (m_send);
		BYTE b_reply[reply_size];
		PICMPV6_ECHO_REPLY reply = (PICMPV6_ECHO_REPLY) b_reply;
		IP_OPTION_INFORMATION ioi = { 0, 0, 0, 0, nullptr };
		for (UCHAR ttl = 1; ttl <= m_max_ttl; ++ttl) {
			ioi.Ttl = ttl;
			f (ttl, 0, tool_StringA::format ("%d", ttl));
			std::string route_ip = "::";
			for (size_t i = 1; i <= m_test_count; ++i) {
				sockaddr_in6 src_addr = { 0 }, dest_addr = { AF_INET6, 0, 0, ul_dest_ip, 0 };
				DWORD dwRetVal = ::Icmp6SendEcho2 (hIcmp, NULL, nullptr, nullptr, &src_addr, &dest_addr, m_send, sizeof (m_send), &ioi, reply, reply_size, 3000);
				DWORD d = GetLastError ();
				if (reply->Status == IP_TTL_EXPIRED_TRANSIT || reply->Status == IP_TTL_EXPIRED_REASSEM || reply->Status == IP_SUCCESS) {
					if (reply->RoundTripTime == 0) {
						f (ttl, i, "<1ms");
					} else {
						f (ttl, i, tool_StringA::format ("%dms", reply->RoundTripTime));
					}
				} else if (reply->Status == IP_REQ_TIMED_OUT) {
					f (ttl, i, "*");
				} else {
					f (ttl, i, "?");
				}
				if (route_ip == "::") {
					char _addr[64] = { 0 };
					inet_ntop (AF_INET6, reply->Address.sin6_addr, _addr, sizeof (_addr) / sizeof (_addr[0]));
					route_ip = _addr;
				}
			}
			f (ttl, 4, route_ip);
			if (dest_ip == route_ip) {
				::IcmpCloseHandle (hIcmp);
				return { true, _T ("路由跟踪完成。") };
			}
		}
		::IcmpCloseHandle (hIcmp);
		return { true, _T ("路由跟踪结束，已超过可跟踪的最大跃点数。") };
	}

private:
	static size_t	m_test_count;
	static UCHAR	m_max_ttl;
	static BYTE		m_send[32];
};

inline size_t		tool_Tracert::m_test_count	= 3;
inline UCHAR		tool_Tracert::m_max_ttl		= 30;
inline BYTE			tool_Tracert::m_send[32] { 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65 };

#endif //__TOOL_TRACERT_HPP__
