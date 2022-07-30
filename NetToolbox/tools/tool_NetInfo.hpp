#ifndef __TOOL_NET_INFO_HPP__
#define __TOOL_NET_INFO_HPP__

#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <memory>

#include <Windows.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <Psapi.h>

#include "tool_String.hpp"
#include "tool_Process.hpp"
#include "tool_Utils.hpp"

#pragma comment(lib, "iphlpapi.lib")



class tool_NetInfo {
public:
	// 获取本地网络的【网络描述、本机地址、网关地址、DHCP】列表
	static std::vector<std::tuple<std::string, std::string, std::string, bool>> get_local_net () {
		std::vector<std::tuple<std::string, std::string, std::string, bool>> ret;
		ULONG size = 0;
		GetAdaptersInfo (NULL, &size);
		if (!size) return ret;
		std::unique_ptr<BYTE[]> buff (new BYTE[size]);
		if (!GetAdaptersInfo ((PIP_ADAPTER_INFO) buff.get (), &size)) {
			PIP_ADAPTER_INFO pStruc = (PIP_ADAPTER_INFO) buff.get ();
			for (; pStruc; pStruc = pStruc->Next) {
				ret.push_back ({ pStruc->Description, pStruc->IpAddressList.IpAddress.String, pStruc->GatewayList.IpAddress.String, !!pStruc->DhcpEnabled });
			}
		}
		return ret;
	}

	// 获取本地网段
	static std::tuple<faw::string_t, faw::string_t> get_ip_segment () {
		ULONG size = 0;
		GetAdaptersInfo (NULL, &size);
		if (!size) return { _T (""), _T ("") };
		std::unique_ptr<BYTE[]> buff (new BYTE[size]);
		if (!GetAdaptersInfo ((PIP_ADAPTER_INFO) buff.get (), &size)) {
			PIP_ADAPTER_INFO pStruc = (PIP_ADAPTER_INFO) buff.get ();
			for (; pStruc; pStruc = pStruc->Next) {
				IPAddr ipaddr = ::inet_addr (pStruc->IpAddressList.IpAddress.String), ipmask = ::inet_addr (pStruc->IpAddressList.IpMask.String);
				size_t mask_bitnum = 0;
				for (size_t i = 0; i < 32; ++i)
					mask_bitnum += (ipmask & (1 << i)) ? 1 : 0;
				if (((ipaddr & ~ipmask) >> mask_bitnum) > 1)
					return { tool_Utils::format_ipv4 ((ipaddr & ipmask) | (1 << mask_bitnum)), tool_Utils::format_ipv4 (ipaddr | ~ipmask) };
			}
		}
		return { _T (""), _T ("") };
	}

	// 获取网络连接列表【是否是ipv4（不显示），本地地址，本地端口，远程地址，远程端口，当前连接状态，进程PID，进程名称，进程路径（不显示）】
	static std::vector<std::tuple<bool, faw::string_t, uint16_t, faw::string_t, uint16_t, faw::string_t, DWORD, faw::string_t>> get_connections () {
		static std::map<DWORD, faw::string_t> mtcp_conn_state {
			//{ MIB_TCP_STATE_CLOSED,			_T ("TCP已关闭") },
			{ MIB_TCP_STATE_LISTEN,			_IT (_T ("TCP Listening")) },
			//{ MIB_TCP_STATE_SYN_SENT,		_T ("TCP连接中") },
			//{ MIB_TCP_STATE_SYN_RCVD,		_T ("TCP连接中") },
			{ MIB_TCP_STATE_ESTAB,			_IT (_T ("TCP Connected")) },
			//{ MIB_TCP_STATE_FIN_WAIT1,		_T ("TCP断开中") },
			//{ MIB_TCP_STATE_FIN_WAIT2,		_T ("TCP断开中") },
			//{ MIB_TCP_STATE_CLOSE_WAIT,		_T ("TCP断开中") },
			//{ MIB_TCP_STATE_CLOSING,		_T ("TCP断开中") },
			//{ MIB_TCP_STATE_LAST_ACK,		_T ("TCP已断开") },
			//{ MIB_TCP_STATE_TIME_WAIT,		_T ("TCP已断开") },
			//{ MIB_TCP_STATE_DELETE_TCB,		_T ("TCP删除TCB记录") },
		};
		std::map<DWORD, faw::string_t> mprocesses = tool_Process::get_processes ();
		auto size_max = [](size_t p1, size_t p2) {
			if (p1 == std::string::npos) {
				return p2;
			} else if (p2 == std::string::npos) {
				return p1;
			} else {
				return (p1 >= p2 ? p1 : p2);
			}
		};
		auto ipv4_to_str = [] (DWORD &addr) -> faw::string_t {
			unsigned char *pch = (unsigned char*) &addr;
			return std::format (_T ("{}.{}.{}.{}"), pch[0], pch[1], pch[2], pch[3]);
		};
		auto ipv6_to_str = [] (UCHAR *addr, DWORD scope_id) -> faw::string_t {
			TCHAR tBuf[64] = { 0 };
			::InetNtop (AF_INET6, &addr, tBuf, sizeof (tBuf) / sizeof (tBuf[0]));
			return (scope_id ? std::format (_T ("{}%{}"), tBuf, scope_id) : faw::string_t (tBuf));
		};

		std::vector<std::tuple<bool, faw::string_t, uint16_t, faw::string_t, uint16_t, faw::string_t, DWORD, faw::string_t>> vconn;
		// tcp ipv4
		ULONG size = 0;
		::GetTcpTable2 (nullptr, &size, true);
		unsigned char *buf = new unsigned char[size];
		PMIB_TCPTABLE2 ptcp4 = (PMIB_TCPTABLE2) buf;
		if (NO_ERROR == ::GetTcpTable2 (ptcp4, &size, true)) {
			for (size_t i = 0; i < ptcp4->dwNumEntries; ++i) {
				faw::string_t local_ip			= ipv4_to_str (ptcp4->table[i].dwLocalAddr);
				uint16_t local_port				= (uint16_t) ::ntohs (ptcp4->table[i].dwLocalPort);
				faw::string_t remote_ip			= ipv4_to_str (ptcp4->table[i].dwRemoteAddr);
				uint16_t remote_port			= (uint16_t) ::ntohs (ptcp4->table[i].dwRemotePort);
				faw::string_t tcp_conn_state		= mtcp_conn_state[ptcp4->table[i].dwState];
				DWORD process_id				= ptcp4->table[i].dwOwningPid;
				faw::string_t exe_path			= mprocesses[process_id];
				if (tcp_conn_state == _T (""))
					continue;
				vconn.push_back ({ true, local_ip, local_port, remote_ip, remote_port, tcp_conn_state, process_id, exe_path });
			}
		}
		delete[] buf;

		// tcp ipv6
		size = 0;
		::GetTcp6Table2 (nullptr, &size, true);
		buf = new unsigned char[size];
		PMIB_TCP6TABLE2 ptcp6 = (PMIB_TCP6TABLE2) buf;
		if (NO_ERROR == ::GetTcp6Table2 (ptcp6, &size, true)) {
			for (size_t i = 0; i < ptcp6->dwNumEntries; ++i) {
				faw::string_t local_ip = ipv6_to_str (ptcp6->table[i].LocalAddr.u.Byte, ptcp6->table[i].dwLocalScopeId);
				uint16_t local_port = (uint16_t) ::ntohs (ptcp6->table[i].dwLocalPort);
				faw::string_t remote_ip = ipv6_to_str (ptcp6->table[i].RemoteAddr.u.Byte, ptcp6->table[i].dwRemoteScopeId);
				uint16_t remote_port = (uint16_t) ::ntohs (ptcp6->table[i].dwRemotePort);
				faw::string_t tcp_conn_state = mtcp_conn_state[ptcp6->table[i].State];
				DWORD process_id = ptcp6->table[i].dwOwningPid;
				faw::string_t exe_path = mprocesses[process_id];
				if (tcp_conn_state == _T (""))
					continue;
				vconn.push_back ({ false, local_ip, local_port, remote_ip, remote_port, tcp_conn_state, process_id, exe_path });
			}
		}
		delete[] buf;

		// udp ipv4
		size = 0;
		::GetExtendedUdpTable (nullptr, &size, true, AF_INET, UDP_TABLE_OWNER_PID, 0);
		buf = new unsigned char[size];
		PMIB_UDPTABLE_OWNER_PID pudp4 = (PMIB_UDPTABLE_OWNER_PID) buf;
		if (NO_ERROR == ::GetExtendedUdpTable (pudp4, &size, true, AF_INET, UDP_TABLE_OWNER_PID, 0)) {
			for (size_t i = 0; i < pudp4->dwNumEntries; ++i) {
				faw::string_t local_ip = ipv4_to_str (pudp4->table[i].dwLocalAddr);
				uint16_t local_port = (uint16_t) ::ntohs (pudp4->table[i].dwLocalPort);
				DWORD process_id = pudp4->table[i].dwOwningPid;
				faw::string_t exe_path = mprocesses[process_id];
				vconn.push_back ({ true, local_ip, local_port, _T ("*"), 0, _T ("UDP"), process_id, exe_path });
			}
		}
		delete[] buf;

		// udp ipv6
		size = 0;
		::GetExtendedUdpTable (nullptr, &size, true, AF_INET6, UDP_TABLE_OWNER_PID, 0);
		buf = new unsigned char[size];
		PMIB_UDP6TABLE_OWNER_PID pudp6 = (PMIB_UDP6TABLE_OWNER_PID) buf;
		if (NO_ERROR == ::GetExtendedUdpTable (pudp6, &size, true, AF_INET6, UDP_TABLE_OWNER_PID, 0)) {
			for (size_t i = 0; i < pudp6->dwNumEntries; ++i) {
				faw::string_t local_ip = ipv6_to_str (pudp6->table[i].ucLocalAddr, pudp6->table[i].dwLocalScopeId);
				uint16_t local_port = (uint16_t) ::ntohs (pudp6->table[i].dwLocalPort);
				DWORD process_id = pudp6->table[i].dwOwningPid;
				faw::string_t exe_path = mprocesses[process_id];
				vconn.push_back ({ true, local_ip, local_port, _T ("*"), 0, _T ("UDP"), process_id, exe_path });
			}
		}
		delete[] buf;
		return vconn;
	}
};

#endif //__TOOL_NET_INFO_HPP__
