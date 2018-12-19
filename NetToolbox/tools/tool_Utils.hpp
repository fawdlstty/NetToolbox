#ifndef __TOOL_UTILS_HPP__
#define __TOOL_UTILS_HPP__

#include <string>
#include <vector>
#include <cstdint>
#include <Windows.h>
#include <tchar.h>

#include "tool_String.hpp"



class tool_Utils {
public:
	static string_t format_unit (double byte_num) {
		LPCTSTR units[] = { _T ("Byte"), _T ("KB"), _T ("MB"), _T ("GB"), _T ("TB"), _T ("PB"), _T ("EB"), _T ("ZB"), _T ("YB"), _T ("NB"), _T ("DB"), _T ("CB") };
		for (size_t i = 0; i < sizeof (units) / sizeof (units[0]); ++i, byte_num /= 1024) {
			if (byte_num < 1024) {
				return tool_StringT::format (_T ("%.2f%s"), byte_num, units[i]);
			}
		}
		return _T ("");
	}

	static string_t get_error_info (DWORD err_no) {
		TCHAR tBuf[MAX_PATH] = { 0 };
		int n = _stprintf_s (tBuf, MAX_PATH, _T ("错误 %d："), err_no);
		if (::FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err_no, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), &tBuf[n], MAX_PATH - n, nullptr) == 0)
			lstrcat (tBuf, _T ("未知错误。"));
		for (; tBuf[n] != _T ('\0') && n < MAX_PATH - 1; ++n) {
			if (tBuf[n] == _T ('\r') || tBuf[n] == _T ('\n')) {
				lstrcpyW (&tBuf[n], &tBuf[n + 1]);
				--n;
			}
		}
		return tBuf;
	}

	static string_t format_ipv4 (IPAddr ip) {
		return tool_StringT::format (_T ("%d.%d.%d.%d"), ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
	}

	static string_t format_ipv4_my (uint32_t ip) {
		return tool_StringT::format (_T ("%d.%d.%d.%d"), (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
	}

	static uint32_t from_ipv4_my (string_t sip) {
		std::vector<string_t> v;
		tool_StringT::split (sip, v, _T ('.'));
		if (v.size () < 4)
			return 0;
		return ((uint32_t) (_ttoi (v[0].c_str ())) << 24) + (_ttoi (v[1].c_str ()) << 16) + (_ttoi (v[2].c_str ()) << 8) + _ttoi (v[3].c_str ());
	}
};

#endif //__TOOL_UTILS_HPP__
