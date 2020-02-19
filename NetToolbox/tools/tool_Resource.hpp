////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Resource
// Description: Resource资源处理工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Feb 19, 2020
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_RESOURCE_HPP__
#define __TOOL_RESOURCE_HPP__

#include <cstdint>
#include <Windows.h>



class tool_Resource {
public:
	// 从资源文件中加载二进制数据
	static uint8_t* load_binary (HMODULE hModule, LPCTSTR resType, LPCTSTR resName) {
		HRSRC _hRsrc = ::FindResource (hModule, resName, resType);
		if (!_hRsrc)
			return nullptr;
		HGLOBAL _hGlobal = ::LoadResource (hModule, _hRsrc);
		if (!_hGlobal)
			return nullptr;
		LPVOID _src_ptr = ::LockResource (_hGlobal);
		DWORD _size = ::SizeofResource (hModule, _hRsrc);
		uint8_t *_ptr = new uint8_t [_size];
		memcpy (_ptr, _src_ptr, _size);
		//::UnlockResource (_hGlobal);
		::FreeResource (_hGlobal);
		return _ptr;
	}
};

#endif //__TOOL_RESOURCE_HPP__
