#ifndef __TOOL_QT_HPP__
#define __TOOL_QT_HPP__

#include <QtCore/QString>
#include <string>

#include <FawLib/FawLib.hpp>



class tool_Qt {
public:
	static QString conv (const char* s) {
		return QString (QString::fromLocal8Bit (s));
	}
	static QString conv (const wchar_t* s) {
		std::string s1 = faw::Encoding::utf16_to_gb18030 (s);
		return QString (QString::fromLocal8Bit (s1.c_str ()));
	}
	static QString conv (std::string s) {
		return conv (s.c_str ());
	}
	static QString conv (std::wstring s) {
		return conv (s.c_str ());
	}
	//static QString conv (string_t s) {
	//	std::string s1 = faw::Encoding::T_to_gb18030 (s);
	//	return QString (QString::fromLocal8Bit (s1.c_str ()));
	//}

	static std::string to_a (QString s) {
		return std::string ((const char *) s.toLocal8Bit ());
	}
	static std::wstring to_w (QString s) {
		std::string s1 = std::string ((const char *) s.toLocal8Bit ());
		return faw::Encoding::gb18030_to_utf16 (s1);
	}
	static faw::string_t_t to_t (QString s) {
		std::string s1 = std::string ((const char *) s.toLocal8Bit ());
		return faw::Encoding::gb18030_to_T (s1);
	}
};

#endif //__TOOL_QT_HPP__
