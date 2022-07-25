#ifndef __TOOL_WMI_HPP__
#define __TOOL_WMI_HPP__

#include <string>
#include <Windows.h>
#include <comutil.h>
#include <WbemIdl.h>

#pragma comment (lib, "wbemuuid.lib")
//#pragma comment (lib, "comsuppw.lib")

// WMI使用的Win32类库名
// http://ilcy2000.blog.sohu.com/132487993.html



class tool_WMI {
public:
	// 获取网卡序列号
	static std::wstring get_netcard_id () {
		return _query_info (L"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))", L"PNPDeviceID");
	}

	// 获取硬盘序列号
	static std::wstring get_disk_id () {
		return _query_info (L"SELECT * FROM Win32_DiskDrive WHERE (SerialNumber IS NOT NULL)", L"SerialNumber");
	}

	// 获取主板序列号
	static std::wstring get_mainboard_id () {
		return _query_info (L"SELECT * FROM Win32_BaseBoard WHERE (SerialNumber IS NOT NULL)", L"SerialNumber");
	}

	// 获取处理器ID
	static std::wstring get_cpu_id () {
		return _query_info (L"SELECT * FROM Win32_Processor WHERE (ProcessorId IS NOT NULL)", L"ProcessorId");
	}

	// 获取BIOS序列号
	static std::wstring get_bios_id () {
		return _query_info (L"SELECT * FROM Win32_BIOS WHERE (SerialNumber IS NOT NULL)", L"SerialNumber");
	}

	// 获取主板型号
	static std::wstring get_mainboard_type () {
		return _query_info (L"SELECT * FROM Win32_BaseBoard WHERE (Product IS NOT NULL)", L"Product");
	}

	// 获取网卡当前MAC地址
	static std::wstring get_netcard_mac () {
		return _query_info (L"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))", L"MACAddress");
	}

protected:
	static std::wstring _query_info (LPCWSTR sql, LPCWSTR prop) {
		std::wstring val = L"";
		IWbemLocator *pLoc = nullptr;
		static bool _init = true;
		if (_init) {
			_init = false;
			::CoInitializeEx (NULL, COINIT_APARTMENTTHREADED);
		}
		if (SUCCEEDED (::CoCreateInstance (CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*) &pLoc))) {
			IWbemServices *pSvc = nullptr;
			if (SUCCEEDED (pLoc->ConnectServer (BSTR (L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc))) {
				if (SUCCEEDED (::CoSetProxyBlanket (pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHN_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE))) {
					IEnumWbemClassObject *pEnum = nullptr;
					if (SUCCEEDED (pSvc->ExecQuery (BSTR (L"WQL"), BSTR (sql), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnum))) {
						IWbemClassObject *pObj = nullptr;
						ULONG retn = 0;
						if (SUCCEEDED (pEnum->Next (WBEM_INFINITE, 1, &pObj, &retn))) {
							// on success, pObj may nullptr
							if (pObj) {
								VARIANT v;
								::VariantInit (&v);
								if (SUCCEEDED (pObj->Get (prop, 0, &v, nullptr, nullptr)))
									val = v.bstrVal;
								::VariantClear (&v);
							}
						}
						pEnum->Release ();
					}
				}
				pSvc->Release ();
			}
			pLoc->Release ();
		}
		return val;
	}
};

#endif //__TOOL_WMI_HPP__
