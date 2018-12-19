#ifndef __TOOL_PATH_HPP__
#define __TOOL_PATH_HPP__

#include <string>
#include <string_view>
#include <vector>
#include <Windows.h>
#include <cstdlib>

#include <openssl/md5.h>

#include "tool_String.hpp"
#include "tool_Encoding.hpp"
#include "tool_Process.hpp"



class tool_Path {
public:
	static std::vector<string_t> &get_args () { return get_obj ().m_args; }
	static string_t get_exe_path () { return get_obj ().m_exe_path; }
	static string_t get_exe_name () { return get_obj ().m_exe_name; }
	static void show_path (string_t path) {
		tool_StringT::replace (path, _T ('/'), _T ('\\'));
		string_t cmd = tool_StringT::format (_T ("/select,\"%s\""), path.c_str ());
		//system (tool_Encoding::get_gb18030 (cmd).c_str ());
		::ShellExecute (NULL, _T ("open"), _T ("explorer.exe"), cmd.c_str (), nullptr, SW_SHOW);
	}

	static bool file_exist (string_view_t file) {
		WIN32_FIND_DATA wfd;
		HANDLE hFind = ::FindFirstFile (file.data (), &wfd);
		bool bExist = (hFind != INVALID_HANDLE_VALUE);
		if (bExist)
			::FindClose (hFind);
		return bExist;
	}
	static bool file_existA (std::string_view file) {
		return file_exist (tool_Encoding::get_T (file));
	}
	static bool file_existW (std::wstring_view file) {
		return file_exist (tool_Encoding::get_T (file));
	}

	static std::string get_file_md5 (LPCTSTR path) {
		HANDLE hFile = ::CreateFile (path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return "";
		DWORD dwsz_high = 0;
		DWORD dwsz = ::GetFileSize (hFile, &dwsz_high);
		::SetFilePointer (hFile, 0, nullptr, FILE_BEGIN);
		int64_t file_length = dwsz | (((int64_t) dwsz_high) * 0x100000000);
		constexpr size_t sz_1M = 1024 * 1024;
		int8_t *buf = new int8_t[sz_1M];
		size_t i, block_count = (size_t) (file_length / sz_1M);
		size_t last_size = (size_t) (file_length - (sz_1M * (int64_t) block_count));
		//
		// 文件Hash
		//
		MD5_CTX _md5;
		::MD5_Init (&_md5);
		//
		for (i = 0; i < block_count; ++i) {
			::ReadFile (hFile, buf, sz_1M, &dwsz, nullptr);
			::MD5_Update (&_md5, buf, sz_1M);
		}
		if (last_size > 0) {
			::ReadFile (hFile, buf, (DWORD) last_size, &dwsz, nullptr);
			::MD5_Update (&_md5, buf, last_size);
		}
		//
		unsigned char buf_md5[16] = { 0 };
		::MD5_Final (buf_md5, &_md5);
		//
		std::string str_md5 = "";
		for (i = 0; i < 64; ++i) {
			if (i < sizeof (buf_md5)) str_md5 += tool_StringA::byte_to_str (buf_md5[i]);
		}
		//
		delete[] buf;
		::CloseHandle (hFile);
		return str_md5;
	}

protected:
	tool_Path () {
		auto cmd_line = ::GetCommandLineW ();
		int _argc = 0;
		LPWSTR *_argv = ::CommandLineToArgvW (cmd_line, &_argc);
		for (int i = 0; i < _argc; ++i)
			m_args.push_back (tool_Encoding::get_T (_argv[i]));
		tool_StringT::replace (m_args[0], _T ('/'), _T ('\\'));
		size_t p = m_args[0].rfind (_T ('\\')) + 1;
		m_exe_path = m_args[0].substr (0, p);
		m_exe_name = m_args[0].substr (p);
	}
	static tool_Path &get_obj () {
		static tool_Path s_obj;
		return s_obj;
	}

	std::vector<string_t>	m_args;
	string_t				m_exe_path;
	string_t				m_exe_name;
};

#endif //__TOOL_PATH_HPP__
