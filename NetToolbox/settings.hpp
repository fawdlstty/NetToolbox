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
		m_file = faw::Directory::get_current_path () + _T ("Settings.json");
		m_fileA = m_file.stra ();
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
		if (bInit) {
			std::ofstream ofs (m_fileA, std::ios::binary);
			ofs << "{}";
			ofs.close ();
		}
	}

private:
	static faw::String m_file;
	static std::string m_fileA;
};

inline faw::String Settings::m_file;
inline std::string Settings::m_fileA;

#endif //__SETTINGS_HPP__
