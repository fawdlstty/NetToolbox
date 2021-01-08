////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_SysInfo
// Description: 系统信息工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_SYS_INFO_HPP__
#define __TOOL_SYS_INFO_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>

#include <Windows.h>
#include <tchar.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <Psapi.h>
//#include <VersionHelpers.h>

#include "tools/tool_String.hpp"
#include "tools/tool_Register.hpp"
#include "tools/tool_Utils.hpp"

#pragma warning (disable: 4996)

class tool_SysInfo {
public:
	//获取电脑品牌
	static std::wstring get_trademark_name () {
		std::wstring facturer, product;
		tool_Register::get_key_value (L"HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS", L"SystemManufacturer", facturer);
		tool_Register::get_key_value (L"HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS", L"SystemProductName", product);
		return tool_StringW::format (L"%s - %s", facturer.c_str (), product.c_str ());
	}

	//获取系统版本信息
	static std::wstring get_system_info () {
		std::wstring sysname = _T ("");
		//OSVERSIONINFOEX osvi = { sizeof (OSVERSIONINFOEX) };
		//DWORDLONG dlcm = 0;
		//int op = VER_GREATER_EQUAL;
		//VER_SET_CONDITION (dlcm, VER_MINORVERSION, op);
		//VER_SET_CONDITION (dlcm, VER_MAJORVERSION, op);
		//VER_SET_CONDITION (dlcm, VER_BUILDNUMBER, op);
		//VER_SET_CONDITION (dlcm, VER_PLATFORMID, op);
		//VER_SET_CONDITION (dlcm, VER_SERVICEPACKMINOR, op);
		//VER_SET_CONDITION (dlcm, VER_SERVICEPACKMAJOR, op);
		//VER_SET_CONDITION (dlcm, VER_SUITENAME, op);
		//VER_SET_CONDITION (dlcm, VER_PRODUCT_TYPE, op);
		//::VerifyVersionInfo (&osvi, 0xff, dlcm);

		OSVERSIONINFOEX osvi = { sizeof (OSVERSIONINFOEX) };
		if (!::GetVersionEx ((OSVERSIONINFO*) &osvi))
			return _T ("");
		SYSTEM_INFO si;
		::GetSystemInfo (&si);
		//processor_num = (int) si.dwNumberOfProcessors;

		switch (osvi.dwPlatformId) {
		case VER_PLATFORM_WIN32_NT:
			if ((osvi.dwMajorVersion == 6 || osvi.dwMajorVersion == 10)) {
				static std::map<std::tuple<int, int, bool>, LPCTSTR> map = {
					{ { 6, 0, true }, _T ("Windows Vista ") },
					{ { 6, 0, false }, _T ("Windows Server 2008 ") },
					{ { 6, 1, true }, _T ("Windows 7 ") },
					{ { 6, 1, false }, _T ("Windows Server 2008 R2 ") },
					{ { 6, 2, true }, _T ("Windows 8 ") },
					{ { 6, 2, false }, _T ("Windows Server 2012 ") },
					{ { 6, 3, true }, _T ("Windows 8.1 ") },
					{ { 6, 3, false }, _T ("Windows Server 2012 R2 ") },
					{ { 10, 0, true }, _T ("Windows 10 ") },
					{ { 10, 0, false }, _T ("Windows Server 2016 ") },
				};
				sysname = map[{ osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.wProductType == VER_NT_WORKSTATION }];
				if (sysname.length () < 1) sysname = _T ("Unknown high version.");
			} else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
				if (GetSystemMetrics (SM_SERVERR2)) {
					sysname = _T ("Microsoft Windows Server 2003 \"R2\" ");
				} else if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
					sysname = _T ("Microsoft Windows XP Professional x64 Edition ");
				} else {
					sysname = _T ("Microsoft Windows Server 2003 ");
				}
			} else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
				sysname = _T ("Microsoft Windows XP ");
			} else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
				sysname = _T ("Microsoft Windows 2000 ");
			} else if (osvi.dwMajorVersion <= 4) {
				sysname = _T ("Microsoft Windows NT ");
			}

			// Test for specific product on Windows NT 4.0 SP6 and later.
			if (sizeof (OSVERSIONINFOEX) == osvi.dwOSVersionInfoSize) {
				if (osvi.wServicePackMajor > 0)
					sysname += tool_StringW::format (_T ("Service Pack %d "), osvi.wServicePackMajor);

				// Test for the workstation type.
				if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64) {
					if (osvi.dwMajorVersion == 4)
						sysname += _T ("Workstation 4.0 ");
					else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
						sysname += _T ("Home Edition ");
					else
						sysname += _T ("Professional ");
				} else if (osvi.wProductType == VER_NT_SERVER || osvi.wProductType == VER_NT_DOMAIN_CONTROLLER) {
					// Test for the server type.
					if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
						if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
							if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
								sysname += _T ("Datacenter Edition for Itanium-based Systems ");
							else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								sysname += _T ("Enterprise Edition for Itanium-based Systems ");
						} else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
							if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
								sysname += _T ("Datacenter x64 Edition ");
							else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								sysname += _T ("Enterprise x64 Edition ");
							else
								sysname += _T ("Standard x64 Edition ");
						} else {
							if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
								sysname += _T ("Datacenter Edition ");
							else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								sysname += _T ("Enterprise Edition ");
							else if (osvi.wSuiteMask & VER_SUITE_BLADE)
								sysname += _T ("Web Edition ");
							else
								sysname += _T ("Standard Edition ");
						}
					} else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
						if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
							sysname += _T ("Datacenter Server ");
						else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
							sysname += _T ("Advanced Server ");
						else
							sysname += _T ("Server ");
					} else { // Windows NT 4.0
						if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
							sysname += _T ("Server 4.0, Enterprise Edition ");
						else
							sysname += _T ("Server 4.0 ");
					}
				}
			} else {
				// Test for specific product on Windows NT 4.0 SP5 and earlier
				std::wstring _tmp;
				tool_Register::get_key_value (_T ("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\ProductOptions"), _T ("ProductType"), _tmp);
				if (lstrcmpi (_T ("WINNT"), &_tmp[0]) == 0)
					sysname += _T ("Workstation ");
				if (lstrcmpi (_T ("LANMANNT"), &_tmp[0]) == 0)
					sysname += _T ("Server ");
				if (lstrcmpi (_T ("SERVERNT"), &_tmp[0]) == 0)
					sysname += _T ("Advanced Server ");
				sysname += tool_StringW::format (_T ("%d.%d "), osvi.dwMajorVersion, osvi.dwMinorVersion);
			}

			// Display service pack (if any) and build number.
			if (osvi.dwMajorVersion == 4 && lstrcmpi (osvi.szCSDVersion, TEXT ("Service Pack 6")) == 0) {
				// Test for SP6 versus SP6a.
				if (tool_Register::path_exist (_T ("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"))) {
					sysname += tool_StringW::format (_T ("Service Pack 6a (Build %d)"), osvi.dwBuildNumber & 0xFFFF);
				} else {// Windows NT 4.0 prior to SP6a
					if (osvi.szCSDVersion[0] != _T ('\0')) {
						sysname += osvi.szCSDVersion;
						sysname += _T (' ');
					}
					sysname += tool_StringW::format (_T ("(Build %d)"), osvi.dwBuildNumber & 0xFFFF);
				}
			} else {// not Windows NT 4.0
				if (osvi.szCSDVersion[0] != _T ('\0')) {
					sysname += osvi.szCSDVersion;
					sysname += _T (' ');
				}
				sysname += tool_StringW::format (_T ("(Build %d)"), osvi.dwBuildNumber & 0xFFFF);
			}
			break;

			// Test for the Windows Me/98/95.
		case VER_PLATFORM_WIN32_WINDOWS:

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
				sysname = _T ("Microsoft Windows 95");
				if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B') sysname += _T (" OSR2");
			} else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
				sysname = _T ("Microsoft Windows 98");
				if (osvi.szCSDVersion[1] == 'A' || osvi.szCSDVersion[1] == 'B') sysname += _T (" SE");
			} else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) {
				sysname = _T ("Microsoft Windows Millennium Edition");
			}
			break;

		case VER_PLATFORM_WIN32s:
			sysname = _T ("Microsoft Win32s");
			break;
		default:
			return _T ("Unknown");
		}
		return sysname;
	}

	//获取CPU信息
	static std::tuple<std::wstring, std::wstring> get_cpu_info () {
		//获取CPU线程数代码在 get_system_info() 函数里面
		std::wstring s1, s2;
		tool_Register::get_key_value (L"HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString", s1);
		tool_Register::get_key_value (L"HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"Identifier", s2);
		return { s1, s2 };
	}

	//获取内存信息
	static faw::String get_memory_info () {
		MEMORYSTATUSEX ms = { sizeof (MEMORYSTATUSEX) };
		if (!::GlobalMemoryStatusEx (&ms))
			return _T ("");
		return faw::String::format (International::translate (_T ("Total %s / Available %s")).data (), tool_Utils::format_unit (ms.ullTotalPhys).c_str (), tool_Utils::format_unit (ms.ullAvailPhys).c_str ());
		//return {
		//	faw::String::format (_T ("总共 %s / 可用 %s"), tool_Utils::format_unit (ms.ullTotalPhys).c_str (), tool_Utils::format_unit (ms.ullAvailPhys).c_str ()),
		//	faw::String::format (_T ("总共 %s / 可用 %s"), tool_Utils::format_unit (ms.ullTotalVirtual).c_str (), tool_Utils::format_unit (ms.ullAvailVirtual).c_str ())
		//};
	}
};

#endif //__TOOL_SYS_INFO_HPP__
