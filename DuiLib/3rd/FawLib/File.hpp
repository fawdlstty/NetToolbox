#////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  File
// Description: 文件类
// Class URI:   https://github.com/fawdlstty/FawLib
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     MIT
// Last Update: Jan 22, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FAWLIB__FILE_HPP__
#define FAWLIB__FILE_HPP__



#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>

#include "String.hpp"
#include "Encoding.hpp"



namespace faw {
	class File {
		File () {}
	public:
		static bool exist (String _path) {
			DWORD _attr = ::GetFileAttributes (_path.c_str ());
			return (_attr != INVALID_FILE_ATTRIBUTES && !(_attr & FILE_ATTRIBUTE_DIRECTORY));
		}
		static void remove (String _path) { ::DeleteFile (_path.c_str ()); }
		static bool copy (String _src, String _dest) { return !!::CopyFile (_src.c_str (), _dest.c_str (), FALSE); }
		static bool write (String _path, String _data, String _encoding = _T ("utf-8"), std::ios::openmode _openmode = std::ios::binary) {
			MakeSureDirectoryPathExists (_path.stra ().c_str ());
			_encoding.lower_self ();
			if (_encoding == _T ("utf-8") || _encoding == _T ("utf8")) {
				std::ofstream ofs (_path.stra (), _openmode);
				if (!ofs.is_open ())
					return false;
				ofs << Encoding::T_to_utf8 (_data.str ());
				ofs.close ();
			} else if (_encoding == _T ("ucs-2") || _encoding == _T ("ucs2") || _encoding == _T ("utf-16") || _encoding == _T ("utf16")) {
				std::wofstream ofs (_path.strw (), _openmode);
				if (!ofs.is_open ())
					return false;
				ofs << Encoding::T_to_utf16 (_data.str ());
				ofs.close ();
			} else if (_encoding == _T ("gb18030") || _encoding == _T ("gbk") || _encoding == _T ("gb2312")) {
				std::ofstream ofs (_path.stra (), _openmode);
				if (!ofs.is_open ())
					return false;
				ofs << Encoding::T_to_gb18030 (_data.str ());
				ofs.close ();
			}
			return true;
		}
		static String read (String _path, String _encoding = _T ("utf-8")) {
			String _ret = _T ("");
			if (_encoding == _T ("utf-8") || _encoding == _T ("utf8")) {
				std::ifstream ifs (_path.stra (), std::ios::binary);
				if (!ifs.is_open ())
					return _ret;
				std::string _data ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
				_ret = Encoding::utf8_to_T (_data);
				ifs.close ();
			} else if (_encoding == _T ("ucs-2") || _encoding == _T ("ucs2") || _encoding == _T ("utf-16") || _encoding == _T ("utf16")) {
				std::wifstream ifs (_path.strw (), std::ios::binary);
				if (!ifs.is_open ())
					return _ret;
				std::string _data ((std::istreambuf_iterator<wchar_t> (ifs)), std::istreambuf_iterator<wchar_t> ());
				_ret = Encoding::utf8_to_T (_data);
				ifs.close ();
			} else if (_encoding == _T ("gb18030") || _encoding == _T ("gbk") || _encoding == _T ("gb2312")) {
				std::ifstream ifs (_path.stra (), std::ios::binary);
				if (!ifs.is_open ())
					return _ret;
				std::string _data ((std::istreambuf_iterator<char> (ifs)), std::istreambuf_iterator<char> ());
				_ret = Encoding::gb18030_to_T (_data);
				ifs.close ();
			}
			return _ret;
		}
	};
}



#endif //FAWLIB__FILE_HPP__
