////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  SetupHelper
// Description: 安装包帮助类
// Class URI:   https://github.com/fawdlstty/FawLib
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     MIT
// Last Update: Mar 20, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FAWLIB__SETUP_HELPER_HPP__
#define FAWLIB__SETUP_HELPER_HPP__



#include <functional>
#include <Windows.h>
#include <ShlObj.h>

#include "Encoding.hpp"
#include "String.hpp"
#include "Register.hpp"
#include "Directory.hpp"
#include "File.hpp"
#include "unzip.h"



namespace faw {
	class SetupHelper {
	public:
		// 构造函数
		SetupHelper (String _app_name, String _app_path = _T ("")): m_app_name (_app_name) {
			if (_app_path.size () > 0) {
				m_app_path = _app_path.replace (_T ("/"), _T ("\\"));
				if (m_app_path [m_app_path.size () - 1] != _T ('\\'))
					m_app_path += _T ('\\');
			} else {
				String _reg_path = String::format (_T ("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), m_app_name.c_str ());
				std::wstring _data;
				if (Register::get_key_value (_reg_path.strw (), L"InstallLocation", _data))
					m_app_path = _data;
			}
		}

		// 资源压缩包解压缩（zip压缩文件）
		size_t install_uncompress (HINSTANCE hInstance, LPCTSTR _resType, LPCTSTR _resId, LPCSTR _zipPwd = nullptr, std::function<void (size_t)> _callback = nullptr) {
			size_t _last = 0, _ret = 0;
			HRSRC hResource = ::FindResource (hInstance, _resId, _resType);
			if (hResource != NULL) {
				HGLOBAL hGlobal = ::LoadResource (hInstance, hResource);
				if (hGlobal) {
					DWORD dwSize = ::SizeofResource (hInstance, hResource);
					if (dwSize) {
						LPVOID pBuffer = ::LockResource (hGlobal);
						HZIP hZip = ::OpenZip (pBuffer, dwSize, _zipPwd);
						ZIPENTRY ze = { 0 };
						GetZipItem (hZip, -1, &ze);
						int _file_count = ze.index;
						Directory::create (m_app_path);
						for (int i = 0; i < _file_count; ++i) {
							GetZipItem (hZip, i, &ze);
							String _tmp = m_app_path + ze.name;
							_tmp.replace_self (_T ('/'), _T ('\\'));
							if (ze.attr & FILE_ATTRIBUTE_DIRECTORY) {
								Directory::create (_tmp, ze.attr);
							} else {
								if (File::exist (_tmp.c_str ()))
									File::remove (_tmp.c_str ());
								//_ret += (UnzipItem (hZip, i, _tmp.c_str ()) == ZR_OK ? 1 : 0);
								if (UnzipItem (hZip, i, _tmp.c_str ()) == ZR_OK)
									_ret += (size_t) (ze.unc_size > 0 ? ze.unc_size : (ze.comp_size >= 0 ? ze.comp_size : 0));
								ze.attr &= (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_NORMAL);
								if (ze.attr)
									::SetFileAttributes (_tmp.c_str (), ze.attr);
							}
							size_t _moment = i * 100 / (size_t) _file_count;
							if (_moment != _last && _callback)
								_callback (_last = _moment);
						}
						if (_callback)
							_callback (_last = 100);
						::CloseZip (hZip);
					}
				}
				::FreeResource (hResource);
			}
			return _ret;
		}

		// 设置注册表
		bool install_initRegester (String _exe_path, String _ico_path, String _desp_name, String _publisher, String _version, DWORD _estimated_size) {
			// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\filezilla.exe
			_exe_path.replace_self (_T ('/'), _T ('\\'));
			String _exe_file = Directory::get_filename (_exe_path);
			String _reg_path = String::format (_T ("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s"), _exe_file.c_str ());
			String _path_value = String::format (_T ("%s%s"), m_app_path.c_str (), _exe_path.c_str ());
			bool bRet = Register::set_path (_reg_path.strw (), _path_value.strw ());
			_path_value = Directory::get_subpath (_path_value.str ());
			bRet = Register::set_key (_reg_path.strw (), L"Path", _path_value.strw ()) && bRet;

			// 计算机\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\GitHubDesktop
			_reg_path = String::format (_T ("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), m_app_name.c_str ());
			std::wstring _reg_path_w = _reg_path.strw ();
			bRet = Register::set_path (_reg_path_w, L"") && bRet;
			_ico_path = String::format (_T ("%s%s"), _path_value.c_str (), _ico_path.c_str ());
			bRet = Register::set_key (_reg_path_w, L"DisplayIcon", _ico_path.strw ()) && bRet;
			bRet = Register::set_key (_reg_path_w, L"DisplayName", _desp_name.strw ()) && bRet;
			bRet = Register::set_key (_reg_path_w, L"DisplayVersion", _version.strw ()) && bRet;
			bRet = Register::set_key (_reg_path_w, L"EstimatedSize", (DWORD) _estimated_size) && bRet;
			char _buf_time [16];
			time_t t = time (NULL);
			strftime (_buf_time, 16, "%Y%m%d", localtime (&t));
			String _time_value { _buf_time };
			bRet = Register::set_key (_reg_path_w, L"InstallDate", _time_value.strw ()) && bRet;
			bRet = Register::set_key (_reg_path_w, L"NoModify", (DWORD) 1) && bRet;
			bRet = Register::set_key (_reg_path_w, L"NoRepair", (DWORD) 1) && bRet;
			bRet = Register::set_key (_reg_path_w, L"Publisher", _publisher.strw ()) && bRet;
			bRet = Register::set_key (_reg_path_w, L"InstallLocation", _path_value.strw ()) && bRet;
			_path_value += _T ("uninstall.exe");
			bRet = Register::set_key (_reg_path_w, L"UninstallString", _path_value.strw ()) && bRet;
			//bRet = Register::set_key (_reg_path_w, L"QuietUninstallString", _path_value.strw ()) && bRet;
			return true;
		}

		// 创建快捷方式
		String install_setShortcut (std::map<String, String> &_desktop, std::map<String, String> &_startmenu) {
			// 创建快捷方式
			auto _set_shortcut = [] (String _dest_file, String _work_path, String _save_path) -> bool {
				IShellLink *psl = nullptr;
				IPersistFile *ppf = nullptr;
				HRESULT _result = ::CoCreateInstance (CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &psl);
				if (FAILED (_result))
					return false;
				_result = psl->QueryInterface (IID_IPersistFile, (void**) &ppf);
				if (SUCCEEDED (_result)) {
					psl->SetPath (_dest_file.c_str ());
					psl->SetWorkingDirectory (_work_path.c_str ());
					psl->SetShowCmd (SW_SHOW);
					ppf->Save (_save_path.strw ().c_str (), TRUE);
				}
				ppf->Release ();
				psl->Release ();
				return true;
			};

			String _ret = _T ("");
			TCHAR buf [MAX_PATH];
			::CoInitializeEx (NULL, COINIT_APARTMENTTHREADED);

			// 桌面快捷方式
			buf [0] = _T ('\0');
			if (::SHGetSpecialFolderPath (NULL, buf, CSIDL_DESKTOP, TRUE)) {
				String _desk_path { buf };
				for (auto &_item : _desktop) {
					String _dest_file = Directory::append_folder_or_file (m_app_path, _item.second);
					String _save_path = Directory::append_folder_or_file (_desk_path, _item.first);
					if (_set_shortcut (_dest_file, m_app_path, _save_path)) {
						_ret += _save_path;
						_ret += _T ('|');
					}
				}
			}

			// 开始菜单快捷方式
			buf [0] = _T ('\0');
			if (::SHGetSpecialFolderPath (NULL, buf, CSIDL_STARTMENU, TRUE)) {
				String _start_path = Directory::append_folder_or_file (buf, _T ("Programs"));
				Directory::create (_start_path);
				for (auto &_item : _startmenu) {
					String _dest_file = Directory::append_folder_or_file (m_app_path, _item.second);
					String _save_path = Directory::append_folder_or_file (_start_path, _item.first);
					if (_set_shortcut (_dest_file, m_app_path, _save_path)) {
						_ret += _save_path;
						_ret += _T ('|');
					}
				}
				_ret += _start_path;
			} else {
				if (_ret [_ret.size () - 1] == _T ('|'))
					_ret.left_self (_ret.size () - 1);
			}

			::CoUninitialize ();
			return _ret;
		}

		// 卸载软件
		void uninstall (String _shortcuts, String _exe_file) {
			// 清理快捷方式
			std::vector<String> _items = _shortcuts.split (_T ('|'));
			for (auto &_item : _items) {
				if (File::exist (_item.c_str ())) {
					File::remove (_item.c_str ());
				} else if (Directory::exist (_item.c_str ())) {
					Directory::remove (_item.c_str ());
				}
			}

			// 清理注册表
			String _reg_path = String::format (_T ("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s"), _exe_file.c_str ());
			Register::delete_path (_reg_path.strw ());
			_reg_path = String::format (_T ("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), m_app_name.c_str ());
			Register::delete_path (_reg_path.strw ());

			// 清理文件
			Directory::remove_nonempty (m_app_path);
		}

		// 获取app名称
		String get_app_name () const { return m_app_name; }

		// 获取app路径
		String get_app_path () const { return m_app_path; }

		// 获取应用程序目录
		static String get_program_files (String _folder) {
			TCHAR buf [MAX_PATH] = _T ("C:\\Program Files");
			::SHGetSpecialFolderPath (NULL, buf, CSIDL_PROGRAM_FILES, TRUE);
			String s { buf };
			s = faw::Directory::append_folder_or_file (s, _folder);
			return s;
		}

		// 获取应用程序版本
		static String get_software_version (String _app_name) {
			String _reg_path = String::format (_T ("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), _app_name.c_str ());
			std::wstring _desp_ver = L"";
			Register::get_key_value (_reg_path.strw (), L"DisplayVersion", _desp_ver);
			return _desp_ver;
		}

		// 获取应用程序所在路径
		static String get_software_location (String _app_name) {
			String _reg_path = String::format (_T ("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), _app_name.c_str ());
			std::wstring _desp_ver = L"";
			Register::get_key_value (_reg_path.strw (), L"InstallLocation", _desp_ver);
			return _desp_ver;
		}

	private:
		String m_app_name = _T ("");
		String m_app_path = _T ("");
	};
}



#endif //FAWLIB__SETUP_HELPER_HPP__
