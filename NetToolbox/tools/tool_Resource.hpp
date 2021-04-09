#ifndef __TOOL_RESOURCE_HPP__
#define __TOOL_RESOURCE_HPP__

#include <cstdint>
#include <tuple>
#include <Windows.h>



class tool_Resource {
public:
	// 从资源文件中加载二进制数据
	static std::tuple<uint8_t*, size_t> load_binary (HMODULE hModule, LPCTSTR resType, LPCTSTR resName) {
		HRSRC _hRsrc = ::FindResource (hModule, resName, resType);
		if (!_hRsrc)
			return { nullptr, 0 };
		HGLOBAL _hGlobal = ::LoadResource (hModule, _hRsrc);
		if (!_hGlobal)
			return { nullptr, 0 };
		LPVOID _src_ptr = ::LockResource (_hGlobal);
		DWORD _size = ::SizeofResource (hModule, _hRsrc);
		uint8_t *_ptr = new uint8_t [_size];
		memcpy (_ptr, _src_ptr, _size);
		//::UnlockResource (_hGlobal);
		::FreeResource (_hGlobal);
		return { _ptr, (size_t) _size };
	}
};

#endif //__TOOL_RESOURCE_HPP__
