////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Register
// Description: 注册表工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_REGISTER_HPP__
#define __TOOL_REGISTER_HPP__

#include <string>
#include <vector>
#include <map>
#include <type_traits>
#include <Windows.h>



class tool_Register {
	tool_Register () = delete;

public:
	static bool set_path (std::wstring path, BYTE *data, DWORD data_len) {
		HKEY main_key = parse_path (path);
		return (ERROR_SUCCESS == ::RegSetValueExW (main_key, path.c_str (), 0, REG_BINARY, data, data_len));
	}
	static bool set_path (std::wstring path, DWORD data) {
		HKEY main_key = parse_path (path);
		return (ERROR_SUCCESS == ::RegSetValueExW (main_key, path.c_str (), 0, REG_DWORD, (BYTE*) &data, sizeof (data)));
	}
	static bool set_path (std::wstring path, std::wstring data, bool expand = false) {
		HKEY main_key = parse_path (path);
		return (ERROR_SUCCESS == ::RegSetValueExW (main_key, path.c_str (), 0, (expand ? REG_EXPAND_SZ : REG_SZ), (BYTE*) &data[0], (DWORD) data.size ()));
	}
	static bool set_path (std::wstring path, std::vector<std::wstring> data) {
		HKEY main_key = parse_path (path);
		std::wstring s = make_multi_sz (data);
		return (ERROR_SUCCESS == ::RegSetValueExW (main_key, path.c_str (), 0, REG_MULTI_SZ, (BYTE*) &s[0], (DWORD) s.size ()));
	}

	static bool set_key (std::wstring path, std::wstring key_name, BYTE *data, DWORD data_len) {
		HKEY hKey = parse_path (path);
		if (ERROR_SUCCESS != ::RegOpenKeyExW (hKey, path.c_str (), 0, KEY_WRITE, &hKey)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegSetValueExW (hKey, key_name.c_str (), 0, REG_BINARY, data, data_len));
		::RegCloseKey (hKey);
		return bRet;
	}
	static bool set_key (std::wstring path, std::wstring key_name, DWORD data) {
		HKEY hKey = parse_path (path);
		if (ERROR_SUCCESS != ::RegOpenKeyExW (hKey, path.c_str (), 0, KEY_WRITE, &hKey)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegSetValueExW (hKey, key_name.c_str (), 0, REG_BINARY, (BYTE*) &data, sizeof (data)));
		::RegCloseKey (hKey);
		return bRet;
	}
	static bool set_key (std::wstring path, std::wstring key_name, std::wstring data, bool expand = false) {
		HKEY hKey = parse_path (path);
		if (ERROR_SUCCESS != ::RegOpenKeyExW (hKey, path.c_str (), 0, KEY_WRITE, &hKey)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegSetValueExW (hKey, key_name.c_str (), 0, (expand ? REG_EXPAND_SZ : REG_SZ), (BYTE*) &data[0], (DWORD) data.size ()));
		::RegCloseKey (hKey);
		return bRet;
	}
	static bool set_key (std::wstring path, std::wstring key_name, std::vector<std::wstring> data) {
		HKEY hKey = parse_path (path);
		if (ERROR_SUCCESS != ::RegOpenKeyExW (hKey, path.c_str (), 0, KEY_WRITE, &hKey)) return false;
		std::wstring s = make_multi_sz (data);
		bool bRet = (ERROR_SUCCESS == ::RegSetValueExW (hKey, key_name.c_str (), 0, REG_MULTI_SZ, (BYTE*) &s[0], (DWORD) s.size ()));
		::RegCloseKey (hKey);
		return bRet;
	}

	static bool get_path_value (std::wstring path, BYTE *&data, DWORD &data_len) {
		HKEY main_key = parse_path (path);
		return (ERROR_SUCCESS == ::RegQueryValueExW (main_key, path.c_str (), nullptr, nullptr, data, &data_len));
	}
	static bool get_path_value (std::wstring path, DWORD &data) {
		HKEY main_key = parse_path (path);
		DWORD data_size = sizeof (data);
		return (ERROR_SUCCESS == ::RegQueryValueExW (main_key, path.c_str (), nullptr, nullptr, (BYTE*) &data, &data_size));
	}
	static bool get_path_value (std::wstring path, std::wstring &data) {
		HKEY main_key = parse_path (path);
		std::wstring _data = L"";
		_data.reserve (4096);
		DWORD sz = (DWORD) 4096 * sizeof (wchar_t);
		bool bRet = (ERROR_SUCCESS == ::RegQueryValueExW (main_key, path.c_str (), nullptr, nullptr, (BYTE*) &_data[0], &sz));
		data = std::wstring (_data.c_str (), sz / 2);
		return bRet;
	}
	static bool get_path_value (std::wstring path, std::vector<std::wstring> &data) {
		HKEY main_key = parse_path (path);
		std::wstring _data = L"";
		_data.reserve (4096);
		DWORD sz = (DWORD) 4096 * sizeof (wchar_t);
		bool bRet = (ERROR_SUCCESS == ::RegQueryValueExW (main_key, path.c_str (), nullptr, nullptr, (BYTE*) &_data[0], &sz));
		data.clear ();
		wchar_t *p1 = &_data[0];
		while (*p1) {
			int len = lstrlenW (p1);
			data.push_back (std::wstring (p1, len));
			p1 += len;
		}
		return bRet;
	}

	static bool get_key_value (std::wstring path, std::wstring key_name, BYTE *&data, DWORD &data_len) {
		HKEY main_key = parse_path (path), sub_key = NULL;
		if (ERROR_SUCCESS != ::RegOpenKeyExW (main_key, path.c_str (), 0, KEY_READ, &sub_key)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegQueryValueExW (sub_key, key_name.c_str (), nullptr, nullptr, data, &data_len));
		::RegCloseKey (sub_key);
		return bRet;
	}
	static bool get_key_value (std::wstring path, std::wstring key_name, DWORD &data) {
		HKEY main_key = parse_path (path), sub_key = NULL;
		DWORD data_size = sizeof (data);
		if (ERROR_SUCCESS != ::RegOpenKeyExW (main_key, path.c_str (), 0, KEY_READ, &sub_key)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegQueryValueExW (sub_key, key_name.c_str (), nullptr, nullptr, (BYTE*) &data, &data_size));
		::RegCloseKey (sub_key);
		return bRet;
	}
	static bool get_key_value (std::wstring path, std::wstring key_name, std::wstring &data) {
		HKEY main_key = parse_path (path), sub_key = NULL;
		std::wstring _data = L"";
		_data.reserve (4096);
		DWORD sz = (DWORD) 4096 * sizeof (wchar_t);
		if (ERROR_SUCCESS != ::RegOpenKeyExW (main_key, path.c_str (), 0, KEY_READ, &sub_key)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegQueryValueExW (sub_key, key_name.c_str (), nullptr, nullptr, (BYTE*) &_data[0], &sz));
		::RegCloseKey (sub_key);
		data = std::wstring (_data.c_str (), sz / 2);
		return bRet;
	}
	static bool get_key_value (std::wstring path, std::wstring key_name, std::vector<std::wstring> &data) {
		HKEY main_key = parse_path (path), sub_key = NULL;
		std::wstring _data = L"";
		_data.reserve (4096);
		DWORD sz = (DWORD) 4096 * sizeof (wchar_t);
		if (ERROR_SUCCESS != ::RegOpenKeyExW (main_key, path.c_str (), 0, KEY_READ, &sub_key)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegQueryValueExW (sub_key, key_name.c_str (), nullptr, nullptr, (BYTE*) &_data[0], &sz));
		::RegCloseKey (sub_key);
		data.clear ();
		wchar_t *p1 = &_data[0];
		while (*p1) {
			int len = lstrlenW (p1);
			data.push_back (std::wstring (p1, len));
			p1 += len;
		}
		return bRet;
	}

	static bool delete_path (std::wstring path) {
		HKEY main_key = parse_path (path);
		return (ERROR_SUCCESS == ::RegDeleteKey (main_key, path.c_str ()));
	}
	static bool delete_key (std::wstring path, std::wstring key_name) {
		HKEY main_key = parse_path (path), sub_key = NULL;
		if (ERROR_SUCCESS != ::RegOpenKeyEx (main_key, path.c_str (), 0, KEY_ALL_ACCESS, &sub_key)) return false;
		bool bRet = (ERROR_SUCCESS == ::RegDeleteKey (sub_key, path.c_str ()));
		::RegCloseKey (sub_key);
		return bRet;
	}

	static bool path_exist (std::wstring path) {
		HKEY main_key = parse_path (path), sub_key = NULL;
		if (ERROR_SUCCESS != ::RegOpenKeyEx (main_key, path.c_str (), 0, KEY_QUERY_VALUE, &sub_key)) return false;
		::RegCloseKey (sub_key);
		return TRUE;
	}

protected:
	static HKEY parse_path (std::wstring &path) {
		static std::map<std::wstring, HKEY> mkeys {
			{ L"HKEY_CLASSES_ROOT",		HKEY_CLASSES_ROOT },
			{ L"HKCR",					HKEY_CLASSES_ROOT },
			{ L"HKEY_CURRENT_USER",		HKEY_CURRENT_USER },
			{ L"HKCU",					HKEY_CURRENT_USER },
			{ L"HKEY_LOCAL_MACHINE",	HKEY_LOCAL_MACHINE },
			{ L"HKLM",					HKEY_LOCAL_MACHINE },
			{ L"HKEY_USERS",			HKEY_USERS },
			{ L"HKU",					HKEY_USERS },
			{ L"HKEY_CURRENT_CONFIG",	HKEY_CURRENT_CONFIG },
			{ L"HKCC",					HKEY_CURRENT_CONFIG },
		};

		size_t p = path.find (L'\\');
		if (p == std::string::npos)
			p = path.size ();
		HKEY mkey = mkeys[path.substr (0, p)];
		path = (path.size () >= p + 1 ? path.substr (p + 1) : L"");
		return mkey;
	}
	static std::wstring make_multi_sz (std::vector<std::wstring> &data) {
		std::wstring s = L"";
		for (auto &_s : data) {
			s += _s;
			s += L'\0';
		}
		s += L'\0';
		return s;
	}
};

#endif //__TOOL_REGISTER_HPP__
