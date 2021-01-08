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

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <winternl.h>
#include <Windows.h>
#include <IcmpAPI.h>
#include <iphlpapi.h>
#include <ip2string.h>

#include <fmt/format.h>

#include "tool_String.hpp"
#include "tool_Utils.hpp"
#include "tool_ObjectPool.hpp"
#include "../../DuiLib/3rd/FawLib/FawLib.hpp"
#include "../International.hpp"

#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

//struct Tracert2Ipv4SentItem;
//static std::vector<Tracert2Ipv4SentItem*> s_sent_items;
//#define _EC_(n) int FAR WINAPI _ec_##n () {					\
//	Tracert2Ipv4SentItem* _pThis = s_sent_items [n];		\
//	_pThis->_on_recv_data ();								\
//	return 0;												\
//}
//_EC_ (0) _EC_ (1) _EC_ (2) _EC_ (3)_EC_ (4) _EC_ (5) _EC_ (6) _EC_ (7) _EC_ (8)_EC_ (9)
//_EC_ (10) _EC_ (11) _EC_ (12) _EC_ (13)_EC_ (14) _EC_ (15) _EC_ (16) _EC_ (17) _EC_ (18)_EC_ (19)
//_EC_ (20) _EC_ (21) _EC_ (22) _EC_ (23)_EC_ (24) _EC_ (25) _EC_ (26) _EC_ (27) _EC_ (28)_EC_ (29)
//std::vector<FARPROC> s_sent_callbacks {
//	_ec_0, _ec_1, _ec_2, _ec_3, _ec_4, _ec_5, _ec_6, _ec_7, _ec_8, _ec_9,
//	_ec_10, _ec_11, _ec_12, _ec_13, _ec_14, _ec_15, _ec_16, _ec_17, _ec_18, _ec_19,
//	_ec_20, _ec_21, _ec_22, _ec_23, _ec_24, _ec_25, _ec_26, _ec_27, _ec_28, _ec_29,
//};



struct Tracert2RecvItem1 {
	IPAddr m_ip = 0;
	size_t m_total_send = 0;
	std::vector<int32_t> m_elapses;

	Tracert2RecvItem1 (IPAddr _ip, size_t _total_send, std::vector<int32_t> &_elapses): m_ip (_ip), m_total_send (_total_send) {
		m_elapses.swap (_elapses);
	}
};

struct Tracert2ViewItem {
	std::string m_ip;
	std::string m_loss;
	std::string m_sent_n;
	std::string m_recv_n;
	std::string m_best;
	std::string m_worst;

	Tracert2ViewItem (Tracert2RecvItem1 &_item) {
		in_addr _addr { 0 };
		_addr.s_addr = _item.m_ip;
		m_ip = ::inet_ntoa (_addr);
		m_loss = fmt::format ("{0:.2f}%", (_item.m_total_send - _item.m_elapses.size ()) / (double) _item.m_total_send * 100);
		m_sent_n = fmt::format ("{}", _item.m_total_send);
		m_recv_n = fmt::format ("{}", _item.m_elapses.size ());
		if (_item.m_elapses.size () > 0) {
			auto _n = std::min_element (_item.m_elapses.cbegin (), _item.m_elapses.cend ());
			if (*_n < 1) {
				m_best = "<1 ms";
			} else {
				m_best = fmt::format ("{} ms", *_n);
			}
			_n = std::max_element (_item.m_elapses.cbegin (), _item.m_elapses.cend ());
			if (*_n < 1) {
				m_worst = "<1 ms";
			} else {
				m_worst = fmt::format ("{} ms", *_n);
			}
		} else {
			m_best = "N/A";
			m_worst = "N/A";
		}
	}
};

//struct Tracert2RecvItem {
//	//in_addr _addr { 0 };
//	//_addr.s_addr = _pThis->m_recv_ptr->Address;
//	//_item.m_ip = ::inet_ntoa (_addr);
//	IPAddr m_ip = 0;
//	int32_t m_elapse = -1;
//};

struct Tracert2Ipv4SentItem {
	struct _sent_item {
		HANDLE m_icmp = ::IcmpCreateFile ();
		uint8_t m_sent_buf [32];
		uint8_t m_recv_buf [sizeof (ICMP_ECHO_REPLY) + sizeof (m_sent_buf)];
		PICMP_ECHO_REPLY m_recv_ptr;
		int32_t m_index = -1;
		std::function<void (int32_t, IPAddr, int32_t)> m_parent_callback;

		_sent_item () {
			memset (m_sent_buf, 0x65, sizeof (m_sent_buf));
			memset (m_recv_buf, 0x65, sizeof (m_recv_buf));
			m_recv_ptr = (PICMP_ECHO_REPLY) m_recv_buf;
		}
		~_sent_item () {
			if (m_icmp != INVALID_HANDLE_VALUE)
				::IcmpCloseHandle (m_icmp);
		}
		void init (int32_t _index, std::function<void (int32_t, IPAddr, int32_t)> _parent_callback) { m_index = _index; m_parent_callback = _parent_callback; }
		bool is_valid () { return m_icmp != INVALID_HANDLE_VALUE; }
		void send_one (PIP_OPTION_INFORMATION _ioi, IPAddr _dest_ip) {
			if (!is_valid ())
				return;
			DWORD dwRetVal = ::IcmpSendEcho2 (m_icmp, NULL, _sent_item::_apc_routine, this, _dest_ip, m_sent_buf, sizeof (m_sent_buf), _ioi, m_recv_buf, sizeof (m_recv_buf), 990);
			DWORD _d = ::GetLastError (); // TODO: 997 重叠 I/O 操作在进行中。 
			_d = _d;
		}
		static void NTAPI _apc_routine (PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, ULONG Reserved) {
			_sent_item *_pThis = (_sent_item*) ApcContext;
			if (IoStatusBlock->Status == 0) {
				auto _status = _pThis->m_recv_ptr->Status;
				if (_status == IP_TTL_EXPIRED_TRANSIT || _status == IP_TTL_EXPIRED_REASSEM || _status == IP_SUCCESS) {
					return _pThis->m_parent_callback (_pThis->m_index, _pThis->m_recv_ptr->Address, _pThis->m_recv_ptr->RoundTripTime);
				}
			}
		}
	};
	std::vector<_sent_item> m_sents;
	//size_t m_ttl;
	IP_OPTION_INFORMATION m_ioi = { 0, 0, 0, 0, nullptr };
	IPAddr m_dest_ip = 0;
	std::function<void (int32_t, Tracert2RecvItem1)> m_recv_callback;

	std::mutex m_mutex;
	IPAddr m_route_ip = 0;
	std::vector<int32_t> m_elapses;

	std::tuple<bool, faw::String> init (size_t _ping_per_second, IPAddr _dest_ip, size_t _ttl) {
		m_sents.resize (_ping_per_second);
		m_dest_ip = _dest_ip;
		//m_ttl = _ttl;
		m_ioi.Ttl = (uint8_t) _ttl;
		for (size_t i = 0; i < m_sents.size (); ++i) {
			if (!m_sents [i].is_valid ())
				return { false, faw::String::format (_T ("IcmpCreateFile %s"), tool_Utils::get_error_info (::GetLastError ()).c_str ()) };
			m_sents [i].init (i, [this] (int _ping_index, IPAddr _route_addr, int32_t _elapses) {
				std::unique_lock<std::mutex> _ul (m_mutex);
				m_elapses.push_back (_elapses);
			});
		}
		return { true, _T ("") };
	}

	void do_send () {
		m_route_ip = 0;
		m_elapses.clear ();
		for (size_t i = 0; i < m_sents.size (); ++i)
			m_sents [i].send_one (&m_ioi, m_dest_ip);
	}

	Tracert2RecvItem1 get_result () {
		return Tracert2RecvItem1 (m_route_ip, m_sents.size (), m_elapses);
	}
};

struct Tracert2CountingItem {
	struct _recv_item {
		size_t m_total_send = 0;
		std::vector<int32_t> m_elapses;

		_recv_item (size_t _total_send, std::vector<int32_t> &_elapses) {
			m_total_send = _total_send;
			m_elapses.assign (_elapses.begin (), _elapses.end ());
		}
	};
	IPAddr m_ip = 0;
	std::vector<_recv_item> m_sents;
};

class tool_Tracert2 {
public:
	tool_Tracert2 () {}
	~tool_Tracert2 () { stop (); }

	void set_view_callback (std::function<void (std::vector<Tracert2ViewItem>)> _set_view) { m_set_view = _set_view; }
	void set_abort_callback (std::function<void (faw::String)> _abort) { m_abort = _abort; }

	// 开始ipv4路由跟踪
	void start_ipv4 (std::string _dest_ip /*TODO: 新增记录日志的参数*/) {
		m_want_run.store (true);
		IPAddr _ul_dest_ip = ::inet_addr (&_dest_ip [0]);
		m_thread = std::move (std::make_unique<std::thread> ([this, _ul_dest_ip] () {
			Tracert2Ipv4SentItem _sents [30];
			Tracert2CountingItem _counts [30]; // TODO: 此结构用于统计数据，但暂未使用
			size_t _counts_length = 0;
			for (size_t _i = 0; _i < 30; ++_i) {
				auto [_result, _reason] = _sents [_i].init (5, _ul_dest_ip, _i + 1);
				if (!_result) {
					m_abort (_reason);
					m_want_run.store (false);
					return;
				}
			}

			// 循环处理，每秒一次
			auto _until_time = std::chrono::system_clock::now ();
			while (m_want_run.load ()) {
				// 向30个链路节点发送跟踪ping
				for (size_t _i = 0; _i < 30; ++_i) {
					_sents [_i].do_send ();
				}
				_until_time += std::chrono::seconds (1);
				std::this_thread::sleep_until (_until_time);

				// 检查本轮结果是否有效
				//bool _value_valid = false;
				std::vector<Tracert2RecvItem1> _cur_recvs;
				for (size_t _i = 0; _i < 30; ++_i) {
					_cur_recvs.push_back (_sents [_i].get_result ());
					decltype (auto) _ref = _cur_recvs.crbegin ();
					//if (_ref->m_elapses.size () > 0)
					//	_value_valid = true;
					if (_ref->m_ip == _ul_dest_ip) {
						break;
					}
				}

				//// 检查链路是否有切换
				bool _switch_link = false;
				if (_counts_length > 0) {
					if (_counts_length != _cur_recvs.size ()) {
						// 链路长度变化
						_switch_link = true;
					} else {
						for (size_t _i = 0; _i < _counts_length; ++_i) {
							if (_counts [_i].m_ip != 0 && _cur_recvs [_i].m_ip != 0 && _counts [_i].m_ip != _cur_recvs [_i].m_ip) {
								// 链路节点变化
								_switch_link = true;
								break;
							}
						}
					}
				}
				_counts_length = _cur_recvs.size ();
				if (_switch_link) {
					// 如果切换链路则清空缓存数据
					for (size_t _i = 0; _i < 30; ++_i) {
						_counts [_i].m_ip = 0;
						_counts [_i].m_sents.clear ();
					}
				}

				// 整理统计数据
				for (size_t _i = 0; _i < _counts_length; ++_i) {
					if (_counts [_i].m_ip == 0)
						_counts [_i].m_ip = _cur_recvs [_i].m_ip;
					_counts [_i].m_sents.emplace_back (Tracert2CountingItem::_recv_item (_cur_recvs [_i].m_total_send, _cur_recvs [_i].m_elapses));
				}
				if (_counts [0].m_sents.size () > 65) {
					for (size_t _i = 0; _i < 30; ++_i)
						_counts [_i].m_sents.erase (_counts [_i].m_sents.begin ());
				}

				// 整理UI显示内容并展示
				std::vector<Tracert2ViewItem> _views;
				for (size_t _i = 0; _i < _counts_length; ++_i) {
					_cur_recvs [_i].m_ip = _counts [_i].m_ip;
					_views.emplace_back (Tracert2ViewItem (_cur_recvs [_i]));
				}
				m_set_view (_views);

				// 是否存日志文件
				// TODO
			}
		}));
	}

	void stop () {
		m_want_run.store (false);
		if (m_thread != nullptr && m_thread->joinable ()) {
			m_thread->join ();
			m_thread = nullptr;
		}
	}

	bool is_running () {
		return m_want_run.load ();
	}

private:
	std::function<void (std::vector<Tracert2ViewItem>)> m_set_view;
	std::unique_ptr<std::thread> m_thread;
	std::function<void (faw::String)> m_abort;
	std::atomic_bool m_want_run = false;
};

#endif //__TOOL_TRACERT2_HPP__
