////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_PE
// Description: PE工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_PE_HPP__
#define __TOOL_PE_HPP__

#include <string>
#include <vector>
#include <tuple>
#include <cstdint>
#include <Windows.h>
#include <DbgHelp.h>

#include "tools/tool_String.hpp"

#pragma comment (lib, "Dbghelp.lib")
#pragma comment (lib, "Version.lib")



class tool_PE {
public:
	// 读取PE文件导入表导出表
	static bool read_import_export (LPCTSTR file_name, std::vector<std::string> &vexport, std::vector<std::tuple<std::string, std::vector<std::tuple<int16_t, std::string>>>> &vimport) {
		vexport.clear ();
		vimport.clear ();
		HANDLE hFile = ::CreateFile (file_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			return false;
		}
		DWORD dwsz_high = 0;
		DWORD dwsz = ::GetFileSize (hFile, &dwsz_high);
		int64_t file_length = dwsz | (((int64_t) dwsz_high) * 0x100000000);
		HANDLE hFileMap = ::CreateFileMapping (hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (hFileMap == NULL) {
			::CloseHandle (hFile);
			return false;
		}
		LPVOID mod_base = ::MapViewOfFile (hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (mod_base == NULL) {
			::CloseHandle (hFileMap);
			::CloseHandle (hFile);
			return false;
		}
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) mod_base;
		if (file_length < (int64_t) sizeof (IMAGE_DOS_HEADER)) {
			::CloseHandle (hFileMap);
			::CloseHandle (hFile);
			return false;
		}
		if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			::CloseHandle (hFileMap);
			::CloseHandle (hFile);
			return false;
		}
		PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS) ((PBYTE) mod_base + pDosHeader->e_lfanew);
		if ((int64_t) pDosHeader->e_lfanew > file_length - (int64_t) sizeof (IMAGE_NT_HEADERS)) {
			::CloseHandle (hFileMap);
			::CloseHandle (hFile);
			return false;
		}
		if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
			::CloseHandle (hFileMap);
			::CloseHandle (hFile);
			return false;
		}
		PIMAGE_OPTIONAL_HEADER pOptHeader = (PIMAGE_OPTIONAL_HEADER) ((PBYTE) mod_base + pDosHeader->e_lfanew + 24);
		//
		DWORD export_virtual_addr = pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		if (export_virtual_addr) {
			PIMAGE_EXPORT_DIRECTORY pExportDesc = (PIMAGE_EXPORT_DIRECTORY) ::ImageRvaToVa (pNtHeader, mod_base, export_virtual_addr, nullptr);
			PDWORD nameAddr = (PDWORD) ::ImageRvaToVa (pNtHeader, mod_base, pExportDesc->AddressOfNames, nullptr);
			for (DWORD i = 0; i < pExportDesc->NumberOfNames && nameAddr; i++) {
				vexport.push_back ((LPCSTR) ::ImageRvaToVa (pNtHeader, mod_base, (DWORD) nameAddr[i], nullptr));
			}
		}
		//
		DWORD import_virtual_addr = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
		if (import_virtual_addr) {
			PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR) ::ImageRvaToVa (pNtHeader, mod_base, import_virtual_addr, nullptr);
			IMAGE_IMPORT_DESCRIPTOR null_iid = { 0 };
			IMAGE_THUNK_DATA null_thunk = { 0 };
			for (DWORD i = 0; memcmp (pImportTable + i, &null_iid, sizeof (null_iid)) != 0; i++) {
				std::string dll_name = (LPCSTR) ::ImageRvaToVa (pNtHeader, mod_base, pImportTable[i].Name, nullptr);
				std::vector<std::tuple<int16_t, std::string>> dll_vimport;
				PIMAGE_THUNK_DATA32 pThunk = (PIMAGE_THUNK_DATA32) ::ImageRvaToVa (pNtHeader, mod_base, pImportTable[i].OriginalFirstThunk, nullptr);
				for (DWORD j = 0; pThunk && memcmp (pThunk + j, &null_thunk, sizeof (null_thunk)) != 0; j++) {
					if (pThunk[j].u1.AddressOfData & IMAGE_ORDINAL_FLAG32) {
						dll_vimport.push_back ({ (uint16_t) (pThunk[j].u1.AddressOfData & 0xffff), "" });
					} else {
						PIMAGE_IMPORT_BY_NAME pFuncName = (PIMAGE_IMPORT_BY_NAME) ::ImageRvaToVa (pNtHeader, mod_base, pThunk[j].u1.AddressOfData, nullptr);
						if (pFuncName) {
							dll_vimport.push_back ({ (uint16_t) pFuncName->Hint, pFuncName->Name });
						}
					}
				}
				vimport.push_back ({ dll_name, dll_vimport });
			}
		}
		CloseHandle (hFileMap);
		CloseHandle (hFile);
		return true;
	}

	// 释放PE文件所有资源
	static faw::String extract_all_resource (LPCTSTR file) {
		HMODULE hModule = ::LoadLibrary (file);
		if (hModule) {
			faw::String folder = faw::String::format (_T ("%s_files"), file);
			::CreateDirectory (folder.c_str (), NULL);
			::EnumResourceTypes (hModule, enum_res_type_proc, (LONG_PTR) folder.c_str ()/*, RESOURCE_ENUM_MUI | RESOURCE_ENUM_LN, NULL*/);
			::FreeLibrary (hModule);
			return folder;
		}
		return _T ("");
	}

	// 获取PE文件版本号
	static std::tuple<size_t, size_t, size_t, size_t> get_version (LPCSTR file) {
		DWORD dw_handle = 0;
		DWORD size = ::GetFileVersionInfoSizeA (file, &dw_handle);
		char *buf = new char[size];
		::GetFileVersionInfoA (file, dw_handle, size, buf);
		VS_FIXEDFILEINFO *pinfo = nullptr;
		UINT ulen = 0;
		::VerQueryValueA (buf, "\\", (LPVOID*) &pinfo, &ulen);
		DWORD verMS = pinfo->dwFileVersionMS;
		DWORD verLS = pinfo->dwFileVersionLS;
		delete buf;
		return { HIWORD (verMS), LOWORD (verMS), HIWORD (verLS), LOWORD (verLS) };
	}

protected:
	// 枚举所有资源名称，供 enum_res_type_proc 函数调用
	static BOOL CALLBACK enum_res_name_proc (HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG_PTR lParam) {
		static auto get_file_name = [] (LPCTSTR lpType, LPTSTR lpName) -> faw::String {
			static std::map<LPCTSTR, LPCTSTR> mtype = { { RT_CURSOR, _T ("RT_CURSOR") }, { RT_BITMAP, _T ("RT_BITMAP") }, { RT_ICON, _T ("RT_ICON") }, { RT_MENU, _T ("RT_MENU") }, { RT_DIALOG, _T ("RT_DIALOG") }, { RT_STRING, _T ("RT_STRING") }, { RT_FONTDIR, _T ("RT_FONTDIR") }, { RT_FONT, _T ("RT_FONT") }, { RT_ACCELERATOR, _T ("RT_ACCELERATOR") }, { RT_RCDATA, _T ("RT_RCDATA") }, { RT_MESSAGETABLE, _T ("RT_MESSAGETABLE") }, { RT_GROUP_CURSOR, _T ("RT_GROUP_CURSOR") }, { RT_GROUP_ICON, _T ("RT_GROUP_ICON") }, { RT_VERSION, _T ("RT_VERSION") }, { RT_DLGINCLUDE, _T ("RT_DLGINCLUDE") }, { RT_PLUGPLAY, _T ("RT_PLUGPLAY") }, { RT_VXD, _T ("RT_VXD") }, { RT_ANICURSOR, _T ("RT_ANICURSOR") }, { RT_ANIICON, _T ("RT_ANIICON") }, { RT_HTML, _T ("RT_HTML") }, { RT_MANIFEST, _T ("RT_MANIFEST") } };
			static std::map<LPCTSTR, LPCTSTR> mext = { { RT_CURSOR, _T ("cur") }, { RT_BITMAP, _T ("bmp") }, { RT_ICON, _T ("ico") }, { RT_MENU, _T ("bin") }, { RT_DIALOG, _T ("bin") }, { RT_STRING, _T ("bin") }, { RT_FONTDIR, _T ("bin") }, { RT_FONT, _T ("font") }, { RT_ACCELERATOR, _T ("bin") }, { RT_RCDATA, _T ("bin") }, { RT_MESSAGETABLE, _T ("bin") }, { RT_GROUP_CURSOR, _T ("cur") }, { RT_GROUP_ICON, _T ("ico") }, { RT_VERSION, _T ("bin") }, { RT_DLGINCLUDE, _T ("bin") }, { RT_PLUGPLAY, _T ("bin") }, { RT_VXD, _T ("vxd") }, { RT_ANICURSOR, _T ("cur") }, { RT_ANIICON, _T ("ico") }, { RT_HTML, _T ("html") }, { RT_MANIFEST, _T ("xml") } };
			faw::String type, ext = _T ("bin");
			DWORD res_id = (DWORD) lpName;
			if (mtype.find (lpType) != mtype.end ()) {
				type = mtype[lpType];
				ext = mext[lpType];
			} else if ((DWORD) lpType < 100) {
				type = faw::String::format (_T ("%d"), (DWORD) lpType);
			} else {
				type = lpType;
				ext = _T ("");
				for (TCHAR c : type)
					ext += (TCHAR) (c >= _T ('A') && c <= _T ('Z') ? c - _T ('A') + _T ('a') : c);
			}
			return faw::String::format (_T ("%s_%d.%s"), type.c_str (), res_id, ext.c_str ());
		};

		//std::cout << "    " << (size_t) lpName << '\n';
		HRSRC hRsrc = ::FindResource (hModule, lpName, lpType);
		if (hRsrc) {
			DWORD dwsz = ::SizeofResource (hModule, hRsrc), written = 0;
			HGLOBAL hGlobal = ::LoadResource (hModule, hRsrc);
			if (hGlobal) {
				LPVOID ptr = ::LockResource (hGlobal);
				if (ptr) {
					LPCTSTR path = (LPCTSTR) lParam;
					faw::String file = faw::String::format (_T ("%s\\%s"), path, get_file_name (lpType, lpName).c_str ());
					HANDLE hFile = ::CreateFile (file.c_str (), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE) {
						::WriteFile (hFile, ptr, dwsz, &written, NULL);
						::CloseHandle (hFile);
					}
					UnlockResource (hGlobal);
				}
				::FreeResource (hGlobal);
			}
		}
		return TRUE;
	}

	// 枚举所有资源类型，供 read_import_export 函数调用
	static BOOL CALLBACK enum_res_type_proc (HMODULE hModule, LPTSTR lpType, LONG_PTR lParam) {
		::EnumResourceNames (hModule, lpType, enum_res_name_proc, lParam/*, RESOURCE_ENUM_MUI | RESOURCE_ENUM_LN, NULL*/);
		return TRUE;
	}
};

#endif //__TOOL_PE_HPP__
