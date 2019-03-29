#////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  Process
// Description: 进程类
// Class URI:   https://github.com/fawdlstty/FawLib
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     MIT
// Last Update: Jan 25, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FAWLIB__PROCESS_HPP__
#define FAWLIB__PROCESS_HPP__



#include <thread>
#include <functional>

#include <Windows.h>
#include <TlHelp32.h>
#include <shellapi.h>
#include <Psapi.h>

#include "String.hpp"

#pragma comment (lib, "psapi.lib")



namespace faw {
	class Process {
		Process () {}
	public:
		static bool create (String _cmd_line, std::function<void ()> _on_exit = nullptr) {
			STARTUPINFO si = { sizeof (STARTUPINFO) };
			PROCESS_INFORMATION pi = { 0 };
			bool bRet = !!::CreateProcess (nullptr, &_cmd_line [0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
			if (!bRet)
				return false;
			CloseHandle (pi.hThread);
			if (_on_exit) {
				HANDLE _handle = pi.hProcess;
				std::thread ([_on_exit, _handle] () {
					DWORD _exit_code = STILL_ACTIVE;
					while (_exit_code == STILL_ACTIVE) {
						std::this_thread::sleep_for (std::chrono::milliseconds (100));
						if (!::GetExitCodeProcess (_handle, &_exit_code))
							break;
					}
					CloseHandle (_handle);
					_on_exit ();
				}).detach ();
			} else {
				CloseHandle (pi.hProcess);
			}
			return true;
		}

		static void shell (String _url) {
			::ShellExecute (NULL, _T ("open"), _url.c_str (), nullptr, nullptr, SW_SHOW);
		}

		// 判断进程是否存在
		static bool process_exist (String file) {
			HANDLE hSnapShot = ::CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 pe32 { sizeof (PROCESSENTRY32) };
			std::map<DWORD, String> m;
			if (::Process32First (hSnapShot, &pe32)) {
				do {
					String _path = pe32.szExeFile;
					_path.replace_self (_T ('/'), _T ('\\'));
					size_t p = _path.rfind (_T ('\\'));
					if (_path.substr (p + 1) == file) {
						::CloseHandle (hSnapShot);
						return true;
					}
				} while (::Process32Next (hSnapShot, &pe32));
			}
			::CloseHandle (hSnapShot);
			return false;
		}
	};
}



#endif //FAWLIB__PROCESS_HPP__
