#ifndef __TOOL_UTILS_HPP__
#define __TOOL_UTILS_HPP__

#include <string>
#include <vector>
#include <cstdint>
#include <Windows.h>
#include <tchar.h>

#include "tools/tool_String.hpp"



class tool_Utils {
public:
	// 格式化计量单位
	static faw::string_t format_unit (double byte_num) {
		LPCTSTR units[] = { _T ("Byte"), _T ("KB"), _T ("MB"), _T ("GB"), _T ("TB"), _T ("PB"), _T ("EB"), _T ("ZB"), _T ("YB"), _T ("NB"), _T ("DB"), _T ("CB") };
		for (size_t i = 0; i < sizeof (units) / sizeof (units[0]); ++i, byte_num /= 1024) {
			if (byte_num < 1024) {
				return fmt::format (_T ("{:.2}{}"), byte_num, units[i]);
			}
		}
		return _T ("");
	}

	// 根据GetLastError的值获取错误信息
	static faw::string_t get_error_info (DWORD err_no) {
		TCHAR tBuf[MAX_PATH] = { 0 };
		int n = _stprintf_s (tBuf, MAX_PATH, International::translate (_T ("Error %d: ")).data (), err_no);
		if (::FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err_no, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), &tBuf[n], MAX_PATH - n, nullptr) == 0)
			lstrcat (tBuf, International::translate (_T ("Unknown Error.")).data ());
		for (; tBuf[n] != _T ('\0') && n < MAX_PATH - 1; ++n) {
			if (tBuf[n] == _T ('\r') || tBuf[n] == _T ('\n')) {
				lstrcpy (&tBuf[n], &tBuf[n + 1]);
				--n;
			}
		}
		return tBuf;
	}

	// 格式化IPv4
	static faw::string_t format_ipv4 (IPAddr ip) {
		return fmt::format (_T ("{}.{}.{}.{}"), ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
	}

	// 格式化IPv4（主机序）
	static faw::string_t format_ipv4_my (uint32_t ip) {
		return fmt::format (_T ("{}.{}.{}.{}"), (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
	}

	// 反格式化IPv4（主机序）
	static uint32_t from_ipv4_my (faw::string_t sip) {
		uint32_t a0 = 0, a1 = 0, a2 = 0, a3 = 0;
		_stscanf (sip.data (), _T ("%u.%u.%u.%u"), &a0, &a1, &a2, &a3);
		return (a0 << 24) + (a1 << 16) + (a2 << 8) + a3;
	}
};

#endif //__TOOL_UTILS_HPP__
