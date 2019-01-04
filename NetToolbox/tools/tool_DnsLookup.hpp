////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_DnsLookup
// Description: DNS查询工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_DNS_LOOKUP_HPP__
#define __TOOL_DNS_LOOKUP_HPP__

#include <iostream>
#include <string>
#include <cstdint>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::udp;

//gethostbyname、getaddrinfo

#define DNS_TYPE_A          0x0001 //1 a host address
#define DNS_TYPE_CNAME      0x0005 //5 the canonical name for an alias



class tool_DnsLookup {
public:
	static std::string query_ip (const char *dest_domain) {
		////getaddrinfo (PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult);
		//addrinfo hints = { 0 }, *result = nullptr;
		//hints.ai_flags = AI_NUMERICHOST;
		//hints.ai_family = AF_UNSPEC;
		//if (::getaddrinfo (dest_domain, nullptr, &hints, &result)) {
		//	DWORD d = ::GetLastError ();
		//	return "";
		//}
		//result = result;
		//::freeaddrinfo (result);
		//return "";
		hostent *host = ::gethostbyname (dest_domain);
		return (host ? (char*) inet_ntoa (*(in_addr*) host->h_addr) : "");
	}
	static std::string query_ipv4_udp (const char *dest_domain, const char *dns_server = "8.8.8.8") {
		char send_packet[MAX_PATH];
		memset (send_packet, 0, sizeof (send_packet));

		//填充DNS查询报文头部
		((uint16_t*) send_packet)[0] = htons (0x1234); // 会话标识
		((uint16_t*) send_packet)[1] = htons (0x0100); // 标识位
		((uint16_t*) send_packet)[2] = htons (0x0001); // Question 个数
		((uint16_t*) send_packet)[3] = htons (0x0000); // Answer 个数
		((uint16_t*) send_packet)[4] = htons (0x0000); // Authority 个数
		((uint16_t*) send_packet)[5] = htons (0x0000); // Additional 个数

		// 填充Question
		std::string query_url = build_query_url (dest_domain);
		memcpy (&send_packet[12], &query_url[0], query_url.size ());
		size_t packet_size = 12 + query_url.size () + 4;
		((uint16_t*) &send_packet[packet_size])[-2] = htons (0x0001);
		((uint16_t*) &send_packet[packet_size])[-1] = htons (0x0001);

		//发送DNS查询报文
		boost::asio::io_service io_service;
		boost::asio::ip::address addr = boost::asio::ip::address::from_string (dns_server);
		udp::endpoint receiver_endpoint (addr, 53);
		udp::socket socket (io_service, udp::endpoint (udp::v4 (), 5264));
		socket.send_to (boost::asio::buffer(send_packet, packet_size), receiver_endpoint);
		boost::array<char, 1024> recv_buf;
		boost::system::error_code err_code;
		socket.receive_from (boost::asio::buffer (recv_buf), receiver_endpoint, 0, err_code);

		const char *recv_packet = recv_buf.data ();
		size_t recv_offset = 12;
		uint16_t session_id		= ntohs (((uint16_t*) recv_packet)[0]);	// 0x1234
		uint16_t flags			= ntohs (((uint16_t*) recv_packet)[1]); // (flags & 0xfb7f) == 0x8100
		uint16_t question_count	= ntohs (((uint16_t*) recv_packet)[2]);
		uint16_t answer_count	= ntohs (((uint16_t*) recv_packet)[3]);

		while (question_count-- > 0)
			std::string reply_url = parse_query_url (recv_packet, recv_offset);

		if (answer_count == 0)
			return "";
		while (answer_count-- > 0) {
			std::string reply_url = parse_query_url (recv_packet, recv_offset);

			uint16_t usAnswerType = ntohs (((uint16_t*) (recv_packet + recv_offset))[0]);
			uint16_t usAnswerClass = ntohs (((uint16_t*) (recv_packet + recv_offset))[1]);
			uint32_t usAnswerTTL = ntohl (((uint32_t*) (recv_packet + recv_offset))[1]);
			uint16_t usAnswerDataLen = ntohs (((uint16_t*) (recv_packet + recv_offset))[4]);
			recv_offset += 10;

			if (usAnswerType == DNS_TYPE_A) {
				uint32_t ulIP = (((uint32_t*) (recv_packet + recv_offset))[0]);
				in_addr addr;
				addr.S_un.S_addr = ulIP;
				return ::inet_ntoa (addr);
				//std::cout << "A:" << inet_ntoa (addr) << "\n";
			} else if (usAnswerType == DNS_TYPE_CNAME) {
				std::string reply_cname = parse_query_url (recv_packet, recv_offset);
				//std::cout << "CNAME:" << reply_cname << "\n";
			}
		}
		return "";
	}

protected:
	static std::string build_query_url (std::string url) {
		std::string query_url = "";
		size_t p1 = 0, p2 = url.find ('.');
		auto _build = [&] () {
			query_url += (char) (p2 - p1);
			for (; p1 < p2; ++p1)
				query_url += url[p1];
		};
		while (p2 != std::string::npos) {
			_build ();
			p2 = url.find ('.', p2 + 1);
			++p1;
		}
		p2 = url.length ();
		_build ();
		query_url += '\0';
		return query_url;
	}

	static std::string parse_query_url (const char *p_url, size_t &offset) {
		std::string url = "";
		bool compress = ((p_url[offset] & 0xc0) == 0xc0);
		size_t _off = (compress ? p_url[offset + 1] : offset);
		while (p_url[_off]) {
			if (url != "")
				url += '.';
			size_t len = (size_t) (p_url[_off++]);
			for (size_t i = 0; i < len; ++i)
				url += p_url[_off++];
		}
		offset = (compress ? offset + 2 : _off + 5);
		return url;
	}
};

#endif //__TOOL_DNS_LOOKUP_HPP__
