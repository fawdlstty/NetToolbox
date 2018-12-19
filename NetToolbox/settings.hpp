#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "tools/tool_Path.hpp"
#include "tools/tool_WebRequest.hpp"
#include "tools/tool_Encoding.hpp"



class Settings {
	Settings () = delete;

public:
	// 初始化
	static void init () {
		m_file = tool_Path::get_exe_path () + _T ("Settings.json");
		m_fileA = tool_Encoding::get_gb18030 (m_file);
		// 运行计数
		bool bInit = !tool_Path::file_exist (m_file);
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
			tool_Encoding::get_gb18030 (m_file);
			std::ofstream ofs (m_fileA, std::ios::binary);
			ofs << "{}";
			ofs.close ();
		}
	}

private:
	static string_t m_file;
	static std::string m_fileA;
};

inline string_t Settings::m_file;
inline std::string Settings::m_fileA;

#endif //__SETTINGS_HPP__
