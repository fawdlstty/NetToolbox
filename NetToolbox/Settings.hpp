#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "tools/tool_WebRequest.hpp"



class Settings {
	Settings () = delete;

public:
	// 初始化
	static void init () {
		m_file = faw::Directory::get_current_path ().str () + _T ("Settings.json");
		// 运行计数
		bool bInit = !faw::Directory::exist (m_file);
#ifndef _DEBUG
		std::thread ([bInit] () {
			if (bInit) {
				tool_WebRequest::get ("https://www.fawdlstty.com:3001/start_count");
			} else {
				tool_WebRequest::get ("https://www.fawdlstty.com:3001/restart_count");
			}
		}).detach ();
#endif
		// 判断操作系统语言，仅临时储存
		::GetSystemDefaultLCID ();
		// 是否第一次运行
		if (bInit) {
			std::ofstream ofs (m_file, std::ios::binary);
			ofs << "{}";
			ofs.close ();
		}
	}

private:
	static faw::string_t m_file;
};

inline faw::string_t Settings::m_file;

#endif //__SETTINGS_HPP__
