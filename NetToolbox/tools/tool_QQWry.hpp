#ifndef __TOOL_QQWRY_HPP__
#define __TOOL_QQWRY_HPP__

#include <string>
#include <fstream>
#include <tuple>
#include <type_traits>
#include <cstdint>
#include <WinSock2.h>

class tool_QQWry {
public:
	tool_QQWry (uint8_t *_bytes): m_bytes (_bytes) {
		m_index_head = read_item_uint32 (0);
		m_index_tail = read_item_uint32 (4);
	}
	~tool_QQWry () { delete [] m_bytes; }

	std::tuple<std::string, std::string> find_info (std::string _ip) {
		uint32_t _nip = ::htonl (::inet_addr (_ip.c_str ()));
		uint32_t _head = m_index_head;
		uint32_t _tail = m_index_tail;
		while (_tail > _head) {
			uint32_t _cur = ((_tail - _head) / 7) >> 1;
			_cur = _head + (_cur == 0 ? 1 : _cur) * 7;
			uint32_t _ip_min = read_item_uint32 (_cur);
			uint32_t _pos = read_item_uint32 (_cur + 4) & 0xffffff;
			uint32_t _ip_max = read_item_uint32 (_pos);
			if (_nip < _ip_min) {
				_tail = _cur;
			} else if (_nip > _ip_max) {
				_head = _cur;
			} else {
				_pos += 4;
				uint8_t _mode = m_bytes [_pos];
				if (_mode == 0x01) {
					uint32_t _main_offset = read_item_uint32 (_pos + 1) & 0xffffff;
					if (m_bytes [_main_offset] == 0x02) {
						return read_infos (_main_offset, _pos + 8);
					} else {
						return read_infos (_main_offset);
					}
				} else if (_mode == 0x02) {
					uint32_t _main_offset = read_item_uint32 (_pos + 1) & 0xffffff;
					return read_infos (_main_offset, _pos + 4);
				} else {
					return read_infos (_pos);
				}
			}
		}
		return std::make_tuple ("", "");
	}

private:
	std::tuple<std::string, std::string> read_infos (uint32_t _main_pos, uint32_t _sub_pos = 0) {
		std::string _main_info = read_item_string (_main_pos);
		_sub_pos = (_sub_pos == 0 ? (_main_pos + (uint32_t) _main_info.size ()) : _sub_pos);
		std::string _sub_info = read_item_string (_sub_pos);
		return std::make_tuple (_main_info, _sub_info);
	}

	uint32_t read_item_uint32 (uint32_t _pos) {
		uint32_t _n = (uint32_t) m_bytes [_pos];
		_n += ((uint32_t) m_bytes [_pos + 1]) << 8;
		_n += ((uint32_t) m_bytes [_pos + 2]) << 16;
		_n += ((uint32_t) m_bytes [_pos + 3]) << 24;
		return _n;
	}

	std::string read_item_string (uint32_t _pos) {
		if (m_bytes [_pos] == 0x01 || m_bytes [_pos] == 0x02) {
			_pos = read_item_uint32 (_pos + 1) & 0xffffff;
			if (m_bytes [_pos] >= 0x00 && m_bytes [_pos] <= 0x02)
				return "";
		}
		std::string _s;
		while (m_bytes [_pos] != 0x00) {
			_s.push_back (m_bytes [_pos++]);
		}
		return _s;
	}

private:
	uint8_t		*m_bytes = nullptr;
	uint32_t	m_index_head = 0;
	uint32_t	m_index_tail = 0;
};

#endif //__TOOL_QQWRY_HPP__
