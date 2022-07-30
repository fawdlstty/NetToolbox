﻿#ifndef __PAGE_FILE_HPP__
#define __PAGE_FILE_HPP__

#include <cstdint>
#include <Windows.h>

#include <boost/crc.hpp>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#include "page_base.hpp"
#include "../tools/tool_String.hpp"
#include "../tools/tool_Utils.hpp"
#include "../tools/tool_PE.hpp"



class page_File: public page_base {
public:
	page_File (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_File () = default;

	bool OnClick (TNotifyUI& msg) override {
		 faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("file_analysis")) {
			 faw::string_t file = m_file_path->GetText ();
			HANDLE hFile = ::CreateFile (file.data (), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _IT (_T ("File not found, cannot be analyzed.")));
				return true;
			}
			m_parent->show_status (NetToolboxWnd::StatusIcon::Loading, _IT (_T ("Analyzing...")));
			DWORD dwsz_high = 0;
			DWORD dwsz = ::GetFileSize (hFile, &dwsz_high);
			::SetFilePointer (hFile, 0, nullptr, FILE_BEGIN);
			int64_t file_length = dwsz | (((int64_t) dwsz_high) * 0x100000000);
			constexpr size_t sz_1M = 1024 * 1024;
			int8_t *buf = new int8_t[sz_1M];
			size_t i, j, block_count = (size_t) (file_length / sz_1M);
			size_t last_size = (size_t) (file_length - (sz_1M * (int64_t) block_count));
			faw::string_t content = std::vformat (_IT (_T ("File path: {}\nFile size: {} byte ({}) \nFile Hash: \n")), std::make_format_args (file, file_length, tool_Utils::format_unit (file_length)));
			//
			// 文件Hash
			//
			boost::crc_32_type crc32;
			MD4_CTX _md4;
			::MD4_Init (&_md4);
			MD5_CTX _md5;
			::MD5_Init (&_md5);
			SHA_CTX _sha1;
			::SHA1_Init (&_sha1);
			SHA256_CTX _sha224;
			::SHA224_Init (&_sha224);
			SHA256_CTX _sha256;
			::SHA256_Init (&_sha256);
			SHA512_CTX _sha384;
			::SHA384_Init (&_sha384);
			SHA512_CTX _sha512;
			::SHA512_Init (&_sha512);
			//
			for (i = 0; i < block_count; ++i) {
				::ReadFile (hFile, buf, sz_1M, &dwsz, nullptr);
				crc32.process_bytes (buf, sz_1M);
				::MD4_Update (&_md4, buf, sz_1M);
				::MD5_Update (&_md5, buf, sz_1M);
				::SHA1_Update (&_sha1, buf, sz_1M);
				::SHA224_Update (&_sha224, buf, sz_1M);
				::SHA256_Update (&_sha256, buf, sz_1M);
				::SHA384_Update (&_sha384, buf, sz_1M);
				::SHA512_Update (&_sha512, buf, sz_1M);
			}
			if (last_size > 0) {
				::ReadFile (hFile, buf, (DWORD) last_size, &dwsz, nullptr);
				crc32.process_bytes (buf, last_size);
				::MD4_Update (&_md4, buf, last_size);
				::MD5_Update (&_md5, buf, last_size);
				::SHA1_Update (&_sha1, buf, last_size);
				::SHA224_Update (&_sha224, buf, last_size);
				::SHA256_Update (&_sha256, buf, last_size);
				::SHA384_Update (&_sha384, buf, last_size);
				::SHA512_Update (&_sha512, buf, last_size);
			}
			//
			unsigned char buf_md4[16] = { 0 }, buf_md5[16] = { 0 }, buf_sha[20] = { 0 }, buf_sha1[20] = { 0 }, buf_sha224[28] = { 0 }, buf_sha256[32] = { 0 }, buf_sha384[48] = { 0 }, buf_sha512[64] = { 0 };
			::MD4_Final (buf_md4, &_md4);
			::MD5_Final (buf_md5, &_md5);
			::SHA1_Final (buf_sha1, &_sha1);
			::SHA224_Final (buf_sha224, &_sha224);
			::SHA256_Final (buf_sha256, &_sha256);
			::SHA384_Final (buf_sha384, &_sha384);
			::SHA512_Final (buf_sha512, &_sha512);
			//
			faw::string_t str_crc32 = std::format (_T ("{:08X}"), crc32.checksum ());
			faw::string_t str_md4 = _T (""), str_md5 = _T (""), str_sha = _T (""), str_sha1 = _T (""), str_sha224 = _T (""), str_sha256 = _T (""), str_sha384 = _T (""), str_sha512 = _T ("");
			for (i = 0; i < 64; ++i) {
				if (i < sizeof (buf_md4)) str_md4 += tool_StringT::byte_to_str (buf_md4[i]);
				if (i < sizeof (buf_md5)) str_md5 += tool_StringT::byte_to_str (buf_md5[i]);
				if (i < sizeof (buf_sha)) str_sha += tool_StringT::byte_to_str (buf_sha[i]);
				if (i < sizeof (buf_sha1)) str_sha1 += tool_StringT::byte_to_str (buf_sha1[i]);
				if (i < sizeof (buf_sha224)) str_sha224 += tool_StringT::byte_to_str (buf_sha224[i]);
				if (i < sizeof (buf_sha256)) str_sha256 += tool_StringT::byte_to_str (buf_sha256[i]);
				if (i < sizeof (buf_sha384)) str_sha384 += tool_StringT::byte_to_str (buf_sha384[i]);
				if (i < sizeof (buf_sha512)) str_sha512 += tool_StringT::byte_to_str (buf_sha512[i]);
			}
			//
			delete[] buf;
			::CloseHandle (hFile);
			//
			content += std::format (_T ("CRC32:     {}\n"), str_crc32);
			content += std::format (_T ("MD4:       {}\n"), str_md4);
			content += std::format (_T ("MD5:       {}\n"), str_md5);
			content += std::format (_T ("SHA:       {}\n"), str_sha);
			content += std::format (_T ("SHA1:      {}\n"), str_sha1);
			content += std::format (_T ("SHA224:    {}\n"), str_sha224);
			content += std::format (_T ("SHA256:    {}\n"), str_sha256);
			content += std::format (_T ("SHA384:    {}\n"), str_sha384);
			content += std::format (_T ("SHA512:    {}\n"), str_sha512);
			//
			// PE分析
			//
			std::vector<std::string> vexport;
			std::vector<std::tuple<std::string, std::vector<std::tuple<int16_t, std::string>>>> vimport;
			if (tool_PE::read_import_export (file.data (), vexport, vimport)) {
				content += _IT (_T ("\n\n\nPE Info：\nExport Function：\n"));
				for (size_t i = 0; i < vexport.size (); ++i) {
					content += faw::Encoding::gb18030_to_T (vexport[i]);
					content += _T ('\n');
				}
				content += _T ("\nImport Function:\n");
				for (i = 0; i < vimport.size (); ++i) {
					auto[dll_name, dll_funcs] = vimport[i];
					content += faw::Encoding::gb18030_to_T (dll_name);
					content += _T ('\n');
					for (j = 0; j < dll_funcs.size (); ++j) {
						auto[func_id, func_name] = dll_funcs[j];
						content += std::format (_T ("\t{}\t{}\n"), func_id, faw::Encoding::gb18030_to_T (func_name));
					}
				}
			} else {
				content += _IT (_T ("\n\n\nFile non-pe file, no PE data.\n"));
			}
			//
			m_file_result->SetText (content.data ());
			m_parent->show_status (NetToolboxWnd::StatusIcon::Ok, _IT (_T ("File analysis completed.")));
			return true;
		}
		return false;
	}

	bool is_accept_drag () override {
		return true;
	}

	bool OnDropFiles (LPCTSTR path) override {
		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFind = ::FindFirstFile (path, &wfd);
		bool bExist = (hFind != INVALID_HANDLE_VALUE);
		if (bExist) {
			::FindClose (hFind);
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _IT (_T ("Folder types are not accepted")));
				return true;
			}
			m_file_path->SetText (path);
		} else {
			m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _IT (_T ("File not found")));
		}
		return true;
	}

protected:
	BindEditUI			m_file_path { _T ("file_path") };
	BindRichEditUI		m_file_result { _T ("file_result") };
};

#endif //__PAGE_FILE_HPP__
