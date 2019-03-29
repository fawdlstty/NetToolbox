#ifndef __TOOL_LOGGING_HPP__
#define __TOOL_LOGGING_HPP__

#include <ctime>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <exception>

#include <FawLib/FawLib.hpp>

class tool_Logging {
public:
	static void info (const char* _file, const int _line, const char* fmt_str, ...) {
		faw::String str_file = faw::Directory::get_filename (_file), str_result = "";
		try {
			int _len = lstrlenA (fmt_str);
			if (_len > 0) {
				va_list ap;
				//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				ptrdiff_t final_n, n = ((ptrdiff_t) _len) * 2;
				std::unique_ptr<char []> formatted;
				while (true) {
					formatted.reset (new char [n]);
					va_start (ap, fmt_str);
					final_n = _vsnprintf_s ((char*) &formatted [0], n, _TRUNCATE, fmt_str, ap);
					va_end (ap);
					if (final_n < 0 || final_n >= n)
						n += abs (final_n - n + 1);
					else
						break;
				}
				str_result = formatted.get ();
			}
		} catch (...) {
		}
		str_result = faw::Encoding::T_to_utf8 (str_result.str_view ());
		//
		//
		//
		static std::mutex m;
		std::lock_guard<std::mutex> lg (m);
		//
		const std::time_t _now = std::time (NULL);
		const tm *_tm = std::localtime (&_now);
		char _buf_file [64] = "log/[time].log", _buf_time [32] = "[time]";
		std::strftime (_buf_file, 64, "log/%Y%m%d.log", _tm);
		std::string stra_path = faw::Encoding::T_to_gb18030 (faw::Directory::get_current_path ().str_view ());
		stra_path += _buf_file;
		//
		std::ofstream ofs (stra_path, std::ios::app);
		std::strftime (_buf_time, 32, "[%Y%m%d_%H%M%S]", _tm);
		sprintf_s (_buf_file, "[%s:%d] ", str_file.c_str (), _line);
		ofs << _buf_time << _buf_file << str_result << std::endl;
		//
		ofs.close ();
	}
	static void info_label (const char* _file, const int _line, const char* _label, const char* fmt_str, ...) {
		faw::String str_file = faw::Directory::get_filename (_file);
		std::string str_result = _T ("");
		try {
			int _len = lstrlenA (fmt_str);
			if (_len > 0) {
				va_list ap;
				//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				ptrdiff_t final_n, n = ((ptrdiff_t) _len) * 2;
				std::unique_ptr<char []> formatted;
				while (true) {
					formatted.reset (new char [n]);
					va_start (ap, fmt_str);
					final_n = _vsnprintf_s ((char*) &formatted [0], n, _TRUNCATE, fmt_str, ap);
					va_end (ap);
					if (final_n < 0 || final_n >= n)
						n += abs (final_n - n + 1);
					else
						break;
				}
				str_result = formatted.get ();
			}
		} catch (...) {
		}
		str_result = faw::Encoding::gb18030_to_utf8 (str_result);
		//
		//
		//
		static std::mutex m;
		std::lock_guard<std::mutex> lg (m);
		//
		const std::time_t _now = std::time (NULL);
		const tm *_tm = std::localtime (&_now);
		char _buf_file [64] = "log/[time].log", _buf_time [32] = "[time]";
		std::strftime (_buf_file, 64, "log/%Y%m%d_", _tm);
		std::string stra_path = faw::Encoding::T_to_gb18030 (faw::Directory::get_current_path ().str_view ());
		stra_path += _buf_file;
		stra_path += _label;
		stra_path += ".log";
		//
		std::ofstream ofs (stra_path, std::ios::app);
		std::strftime (_buf_time, 32, "[%Y%m%d_%H%M%S]", _tm);
		sprintf_s (_buf_file, "[%s:%d] ", str_file.c_str (), _line);
		ofs << _buf_time << _buf_file << str_result << std::endl;
		//
		ofs.close ();
	}
};

#define LOG_INFO(...) tool_Logging::info(__FILE__,__LINE__,__VA_ARGS__)
#define LOG_INFO_LABEL(...) tool_Logging::info_label(__FILE__,__LINE__,__VA_ARGS__)
//#define LOG_INFO tool_Logging::info

#endif //__TOOL_LOGGING_HPP__
