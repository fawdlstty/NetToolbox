#////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  Directory
// Description: 文件夹类
// Class URI:   https://github.com/fawdlstty/FawLib
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     MIT
// Last Update: May 16, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FAWLIB__DIRECTORY_HPP__
#define FAWLIB__DIRECTORY_HPP__



#include <string>
#include <Windows.h>

#include <DbgHelp.h>
#pragma comment (lib, "DbgHelp.lib")
#include <shellapi.h>
#pragma comment (lib, "shell32.lib")

#include "String.hpp"
#include "File.hpp"



namespace faw {
	class Directory {
		Directory () {}
	public:
		static bool exist (String _path) {
			DWORD _attr = ::GetFileAttributes (_path.c_str ());
			return (_attr != INVALID_FILE_ATTRIBUTES && _attr & FILE_ATTRIBUTE_DIRECTORY);
		}
		static void remove (String _path) {
			if (_path.at (-1) == _T ('/') || _path.at (-1) == _T ('\\'))
				_path = _path.left (_path.size () - 1);
			::RemoveDirectory (_path.c_str ());
		}
		static void remove_nonempty (String _path) {
			if (_path.empty ())
				return;
			String _path_find = append_folder_or_file (_path, _T ("*"));
			WIN32_FIND_DATA wfd { 0 };
			HANDLE hFind = ::FindFirstFile (_path_find.c_str (), &wfd);
			if (hFind == INVALID_HANDLE_VALUE)
				return;
			do {
				String _dest { wfd.cFileName };
				if (_dest == _T (".") || _dest == _T (".."))
					continue;
				_dest = append_folder_or_file (_path, _dest);
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					remove_nonempty (_dest);
				} else {
					File::remove (_dest);
				}
			} while (::FindNextFile (hFind, &wfd));
			remove (_path);
		}
		static bool create (String _path, DWORD _attr = 0) {
			//bool bRet = (exist (_path) ? true : !!::CreateDirectory (_path.c_str (), NULL));
			//if (bRet) {
			//	_attr &= (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_NORMAL);
			//	if (_attr)
			//		::SetFileAttributes (_path.c_str (), _attr);
			//}
			//return bRet;
			_path.replace_self (_T ('/'), _T ('\\'));
			if (_path.at (-1) != _T ('\\'))
				_path += _T ('\\');
			bool bRet = !!::MakeSureDirectoryPathExists (_path.stra ().c_str ());
			if (bRet && _attr) {
				//DWORD _attr_old = ::GetFileAttributes (_path.c_str ());
				_attr &= (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_NORMAL);
				::SetFileAttributes (_path.c_str (), _attr | FILE_ATTRIBUTE_DIRECTORY);
			}
			return bRet;
		}
		static String get_subpath (String _path) {
			size_t _p = _path.rfind_any ({ _T ('/'), _T ('\\') });
			if (_p == _path.size () - 1) {
				_path.left_self (_path.size () - 1);
				_p = _path.rfind_any ({ _T ('/'), _T ('\\') });
			}
			if (_p == String::_npos)
				return _T ("");
			return _path.substr (0, _p + 1);
		}
		static String get_filename (String _path) {
			size_t _p = _path.rfind_any ({ _T ('/'), _T ('\\') });
			if (_p == String::_npos)
				return _path;
			return _path.substr (_p + 1);
		}
		static String get_last_folder (String _path) {
			size_t _p = _path.rfind_any ({ _T ('/'), _T ('\\') });
			if (_p == _path.size () - 1) {
				_path.left_self (_path.size () - 1);
				_p = _path.rfind_any ({ _T ('/'), _T ('\\') });
			}
			if (_p == String::_npos)
				return _path;
			return _path.substr (_p + 1);
		}
		static String get_current_file () {
			String _s { ::GetCommandLine () };
			TCHAR _end_ch = _T (' ');
			if (_s[0] == _T ('"'))
				_end_ch << _s;
			return _s.left (_s.find (_end_ch));
		}
		static String get_current_path () {
			String _s = get_current_file ();
			size_t _p = _s.rfind_any ({ _T ('/'), _T ('\\') });
			return _s.left (_p + 1);
		}
		static String get_current_file1 (HMODULE hModule) {
			TCHAR buf [MAX_PATH] = { 0 };
			::GetModuleFileName (hModule, buf, MAX_PATH);
			String _s { buf };
			TCHAR _end_ch = _T (' ');
			if (_s [0] == _T ('"'))
				_end_ch << _s;
			return _s.left (_s.find (_end_ch));
		}
		static String get_current_path1 (HMODULE hModule) {
			String _s = get_current_file1 (hModule);
			size_t _p = _s.rfind_any ({ _T ('/'), _T ('\\') });
			return _s.left (_p + 1);
		}
		static String append_folder_or_file (String _path, String _dest) {
			String s { _path };
			if (s.rfind_any ({ _T ('/'), _T ('\\') }) != s.size () - 1)
				s += _T ('\\');
			s += _dest;
			return s;
		}
		static String get_temp_file (String _file) {
			TCHAR _buf [MAX_PATH], _buf2 [MAX_PATH];
			_buf [0] = _buf2 [0] = _T ('\0');
			if (!::GetTempPath (MAX_PATH, _buf))
				lstrcpy (_buf, _T ("D:\\"));
			if (!::GetFullPathName (_buf, MAX_PATH, _buf2, nullptr))
				lstrcpy (_buf2, _T ("D:\\"));
			return append_folder_or_file (_buf, _file);
		}
		static void show_path (String path) {
			path.replace_self (_T ('/'), _T ('\\'));
			String cmd = String::format (_T ("/select,\"%s\""), path.c_str ());
			::ShellExecute (NULL, _T ("open"), _T ("explorer.exe"), cmd.c_str (), nullptr, SW_SHOW);
		}

	};
}



#endif //FAWLIB__DIRECTORY_HPP__
