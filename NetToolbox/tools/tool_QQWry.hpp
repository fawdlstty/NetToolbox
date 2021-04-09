#ifndef __TOOL_QQWRY_HPP__
#define __TOOL_QQWRY_HPP__

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include <Windows.h>

class tool_QQWry {
public:
	tool_QQWry (std::string _file) {
		std::ifstream ifs (_file, std::ios::binary);
		//m_bytes = std::string ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ()); // low performance
		ifs.seekg (0, std::ios::end);
		size_t _fsz = (size_t) ifs.tellg ();
		ifs.seekg (0, std::ios::beg);
		m_bytes.resize (_fsz);
		ifs.read (&m_bytes [0], (std::streamsize) _fsz);
		m_index_head = read_item_uint32 (0);
		m_index_tail = read_item_uint32 (4);
	}

	tool_QQWry (std::tuple<uint8_t*, size_t> _data) {
		m_bytes.resize (std::get<1> (_data));
		memcpy (m_bytes.data (), std::get<0> (_data), std::get<1> (_data));
		delete[] std::get<0> (_data);
		m_index_head = read_item_uint32 (0);
		m_index_tail = read_item_uint32 (4);
	}

	std::tuple<std::string, std::string, std::string> find_info (std::string _ip) {
		uint32_t _ip0, _ip1, _ip2, _ip3;
		sscanf_s (_ip.data (), "%u.%u.%u.%u", &_ip0, &_ip1, &_ip2, &_ip3);
		uint32_t _nip = (_ip0 << 24) + (_ip1 << 16) + (_ip2 << 8) + _ip3;
		uint32_t _head = m_index_head;
		uint32_t _tail = m_index_tail;
		while (_tail > _head) {
			uint32_t _cur = ((_tail - _head) / 7) >> 1;
			_cur = _head + (_cur == 0 ? 1 : _cur) * 7;
			uint32_t _ip_min = read_item_uint32 (_cur);
			uint32_t _pos = read_item_uint24 (_cur + 4);
			uint32_t _ip_max = read_item_uint32 (_pos);
			if (_nip < _ip_min) {
				_tail = _cur;
			} else if (_nip > _ip_max) {
				_head = _cur;
			} else {
				_pos += 4;
				uint8_t _mode = m_bytes [_pos];
				std::string _info0, _info1, _desp = "";
				if (_mode == 0x01) {
					uint32_t _main_offset = read_item_uint24 (_pos + 1);
					if (m_bytes [_main_offset] == 0x02) {
						std::tie (_info0, _info1) = read_infos (_main_offset, _pos + 8);
						_desp = read_area (_main_offset + 4);
					} else {
						std::tie (_info0, _info1) = read_infos (_main_offset);
						_desp = read_area (_main_offset + (uint32_t) _info0.size () + 1);
					}
				} else if (_mode == 0x02) {
					uint32_t _main_offset = read_item_uint24 (_pos + 1);
					std::tie (_info0, _info1) = read_infos (_main_offset, _pos + 4);
					_desp = read_area (_pos + 8);
				} else {
					std::tie (_info0, _info1) = read_infos (_pos);
					_desp = read_area (_pos + (uint32_t) _info0.size () + 1);
				}
				return std::make_tuple (_info0, _info1, _desp);
			}
		}
		return std::make_tuple ("", "", "");
	}

private:
	std::tuple<std::string, std::string> read_infos (uint32_t _main_pos, uint32_t _sub_pos = 0) {
		std::string _main_info = read_item_string (_main_pos);
		_sub_pos = (_sub_pos == 0 ? (_main_pos + (uint32_t) _main_info.size ()) : _sub_pos);
		std::string _sub_info = read_item_string (_sub_pos);
		return std::make_tuple (_main_info, _sub_info);
	}

	std::string read_area (uint32_t _area_pos) {
		uint8_t _mode = m_bytes [_area_pos];
		if (_mode == 1 || _mode == 2) {
			_area_pos = read_item_uint24 (_area_pos + 1);
			if (_area_pos == 0) {
				return "";
			} else {
				return read_item_string (_area_pos);
			}
		} else {
			return read_item_string (_area_pos);
		}
	}

	uint32_t read_item_uint24 (uint32_t _pos) {
		uint32_t _n = 0xff & (uint32_t) m_bytes [_pos];
		_n += (0xff & (uint32_t) m_bytes [_pos + 1]) << 8;
		_n += (0xff & (uint32_t) m_bytes [_pos + 2]) << 16;
		return _n;
	}

	uint32_t read_item_uint32 (uint32_t _pos) {
		uint32_t _n = 0xff & (uint32_t) m_bytes [_pos];
		_n += (0xff & (uint32_t) m_bytes [_pos + 1]) << 8;
		_n += (0xff & (uint32_t) m_bytes [_pos + 2]) << 16;
		_n += (0xff & (uint32_t) m_bytes [_pos + 3]) << 24;
		return _n;
	}

	std::string read_item_string (uint32_t _pos) {
		if (m_bytes [_pos] == 0x01 || m_bytes [_pos] == 0x02) {
			_pos = read_item_uint24 (_pos + 1);
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
	std::string				m_bytes;//std::vector<uint8_t>
	uint32_t				m_index_head = 0;
	uint32_t				m_index_tail = 0;
};

#endif //__TOOL_QQWRY_HPP__
