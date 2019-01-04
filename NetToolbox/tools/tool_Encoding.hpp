////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Encoding
// Description: 编码工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_ENCODING_HPP__
#define __TOOL_ENCODING_HPP__

#include <string>
#include <vector>
#include <Windows.h>

#ifndef _FAW_STRING_TYPE
#define _FAW_STRING_TYPE
#ifdef UNICODE
typedef std::wstring string_t;
#else
typedef std::string string_t;
#endif

#define _USE_STRING_VIEW
#ifdef _USE_STRING_VIEW
#include <string_view>
#ifdef UNICODE
typedef std::wstring_view string_view_t;
#else
typedef std::string_view string_view_t;
#endif
#else //_USE_STRING_VIEW
namespace std {
	typedef string string_view;
	typedef wstring wstring_view;
}
typedef string_t string_view_t;
#endif //_USE_STRING_VIEW
#endif



class tool_Encoding {
public:
	// 编码猜测
	static std::string guess (unsigned char *data, int length) {
		if (is_ascii (data, length)) {
			return "ascii";
		} else if (is_utf8 (data, length)) {
			return "utf8";
		} else if (is_gb18030 (data, length)) {
			return "gb18030";
		} else if (is_utf16 (data, length)) {
			return "utf16";
		} else {
			return "iso-8859-1";
		}
	}
	static bool is_ascii (unsigned char *data, int length) {
		while (--length >= 0 && !(data[length] & 0x80));
		return length < 0;
	}
	static bool is_utf8 (unsigned char *data, int length) {
		auto high_len = [] (unsigned char ch) {
			int _len = 0;
			while (ch & 0x80) {
				++_len;
				ch <<= 1;
			}
			return _len;
		};
		int state = 0;
		for (int i = 0; i < length; ++i) {
			int _len = high_len (data[i]);
			if (state > 0) {
				if (_len != 1)
					return false;
				--state;
			} else if (_len == 1) {
				return false;
			} else if (_len > 1) {
				state = _len - 1;
			}
		}
		return (state == 0);
	}
	static bool is_gb18030 (unsigned char *data, int length) {
		int state = 0;
		for (int i = 0; i < length; ++i) {
			if (data[i] & 0x80) {
				state = 1 - state;
			} else if (state == 1) {
				return false;
			}
		}
		return true;
	}
	static bool is_utf16 (unsigned char *data, int length) {
		if (length % 2)
			return false;
		for (int i = 0; i < length; i += 2) {
			if ((data[i] & 0x80) != (data[i + 1] & 0x80))
				return false;
		}
		return true;
	}
	static bool is_base64 (const unsigned char *data, int length) {
		if (length % 4) {
			return false;
		} else if (length == 0) {
			return true;
		}
		if (data[length - 1] == '=')
			--length;
		if (data[length - 1] == '=')
			--length;
		for (int i = 0; i < length; ++i) {
			char ch = data[i];
			if (!(isalnum (ch) || (ch == '+') || (ch == '/')))
				return false;
		}
		return true;
	}

	// 编码转换
	static std::wstring gb18030_to_utf16 (std::string_view _old) { return _conv_to_wide (_old, CP_ACP); }
	static std::string utf16_to_gb18030 (std::wstring_view _old) { return _conv_to_multi (_old, CP_ACP); }
	static std::wstring utf8_to_utf16 (std::string_view _old) { return _conv_to_wide (_old, CP_UTF8); }
	static std::string utf16_to_utf8 (std::wstring_view _old) { return _conv_to_multi (_old, CP_UTF8); }
	static std::string gb18030_to_utf8 (std::string_view _old) { return utf16_to_utf8 (gb18030_to_utf16 (_old)); }
	static std::string utf8_to_gb18030 (std::string_view _old) { return utf16_to_gb18030 (utf8_to_utf16 (_old)); }
#ifdef UNICODE
	static std::string T_to_gb18030 (string_view_t _old) { return utf16_to_gb18030 (_old); }
	static std::string T_to_utf8 (string_view_t _old) { return utf16_to_utf8 (_old); }
	static std::wstring T_to_utf16 (string_view_t _old) { return std::wstring (_old); }
	static string_t gb18030_to_T (std::string_view _old) { return gb18030_to_utf16 (_old); }
	static string_t utf8_to_T (std::string_view _old) { return utf8_to_utf16 (_old); }
	static string_t utf16_to_T (std::wstring_view _old) { return string_t (_old); }
#else
	static std::string T_to_gb18030 (string_view_t _old) { return std::string (_old); }
	static std::string T_to_utf8 (string_view_t _old) { return gb18030_to_utf8 (_old); }
	static std::wstring T_to_utf16 (string_view_t _old) { return gb18030_to_utf16 (_old); }
	static string_t gb18030_to_T (std::string_view _old) { return string_t (_old); }
	static string_t utf8_to_T (std::string_view _old) { return utf8_to_gb18030 (_old); }
	static string_t utf16_to_T (std::wstring_view _old) { return utf16_to_gb18030 (_old); }
#endif
private:
	static std::string _conv_to_multi (std::wstring_view _old, UINT ToType) {
		int lenOld = lstrlenW (_old.data ());
		int lenNew = ::WideCharToMultiByte (ToType, 0, _old.data (), lenOld, nullptr, 0, nullptr, nullptr);
		std::string s;
		s.resize (lenNew);
		if (!::WideCharToMultiByte (ToType, 0, _old.data (), lenOld, const_cast<char*>(s.c_str ()), lenNew, nullptr, nullptr))
			return "";
		return s.c_str ();
	}
	static std::wstring _conv_to_wide (std::string_view _old, UINT ToType) {
		int lenOld = lstrlenA (_old.data ());
		int lenNew = ::MultiByteToWideChar (ToType, 0, _old.data (), lenOld, nullptr, 0);
		std::wstring s;
		s.resize (lenNew);
		if (!::MultiByteToWideChar (ToType, 0, _old.data (), lenOld, const_cast<wchar_t*>(s.c_str ()), lenNew))
			return L"";
		return s.c_str ();
	}
	static const std::string base64_chars;

public:
	static bool is_hex_char (char ch) {
		return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
	}
	static bool is_base64_char (char ch) {
		return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '+' || ch == '/';
	}
	static char hex_char_to_dec (char ch) {
		if (ch >= '0' && ch <= '9') {
			return ch - '0';
		} else if (ch >= 'A' && ch <= 'F') {
			return ch - 'A' + 10;
		} else if (ch >= 'a' && ch <= 'f') {
			return ch - 'a' + 10;
		} else {
			return 0;
		}
	}

	// 格式猜测
	static bool is_percent_str (std::string_view data) {
		bool ret = false;
		for (size_t i = 0; i < data.size (); ++i) {
			if (data[i] == '%') {
				if (i + 1 >= data.size ())
					return false;
				if (data[++i] == '%')
					continue;
				if (i + 1 >= data.size ())
					return false;
				if (!is_hex_char (data[i]))
					return false;
				if (!is_hex_char (data[++i]))
					return false;
				ret = true;
			}
		}
		return ret;
	}
	static bool is_escape_x_str (std::string_view data) {
		bool ret = false;
		for (size_t i = 0; i < data.size (); ++i) {
			if (data[i] == '\\') {
				if (i + 1 >= data.size ())
					return false;
				if (data[++i] == '\\')
					continue;
				if (data[i] != 'x')
					continue;
				if (i + 2 >= data.size ())
					return false;
				if (!is_hex_char (data[++i]))
					return false;
				if (!is_hex_char (data[++i]))
					return false;
				ret = true;
			}
		}
		return ret;
	}
	static bool is_escape_u_str (std::string_view data) {
		bool ret = false;
		for (size_t i = 0; i < data.size (); ++i) {
			if (data[i] == '\\') {
				if (i + 1 >= data.size ())
					return false;
				if (data[++i] == '\\')
					continue;
				if (data[i] != 'u')
					continue;
				if (i + 4 >= data.size ())
					return false;
				if (!is_hex_char (data[++i]))
					return false;
				if (!is_hex_char (data[++i]))
					return false;
				if (!is_hex_char (data[++i]))
					return false;
				if (!is_hex_char (data[++i]))
					return false;
				ret = true;
			}
		}
		return ret;
	}
	static bool is_base64_str (std::string_view data) {
		for (size_t i = 0; i < data.size (); ++i) {
			if (!is_base64_char (data[i])) {
				if (i < data.size () - 2 || data[i] != '=')
					return false;
			}
		}
		return true;
	}

	// 格式转换
	static std::string percent_str_encode (std::string_view data) {
		const static char *hex_char = "0123456789ABCDEF";
		std::string ret = "";
		for (size_t i = 0; i < data.size (); ++i) {
			char ch = data[i];
			if (isalnum ((unsigned char) ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
				ret += ch;
			} else if (ch == ' ') {
				ret += "+";
			} else {
				ret += '%';
				ret += hex_char [((unsigned char) ch) >> 4];
				ret += hex_char [((unsigned char) ch) % 16];
			}
		}
		return ret;
	}
	static std::string percent_str_decode (std::string_view data) {
		std::string ret = "";
		for (size_t i = 0; i < data.size (); ++i) {
			char ch = data[i];
			if (ch == '%') {
				ret += (char) ((hex_char_to_dec (data[i + 1]) << 4) | hex_char_to_dec (data[i + 2]));
				i += 2;
			} else {
				ret += ch;
			}
		}
		return ret;
	}
	static std::string escape_x_str_encode (std::string_view data) {
		const static char *hex_char = "0123456789ABCDEF";
		std::string ret = "";
		for (size_t i = 0; i < data.size (); ++i) {
			char ch = data[i];
			if ((ch >= 0x20 && ch <= 0x7e) || ch == '\x09' || ch == '\x0a' || ch == '\x0b' || ch == '\x0d') {
				ret += ch;
			} else {
				ret += "\\x";
				ret += hex_char[((unsigned char) ch) >> 4];
				ret += hex_char[((unsigned char) ch) % 16];
			}
		}
		return ret;
	}
	static std::string escape_x_str_decode (std::string_view data) {
		std::string ret = "";
		for (size_t i = 0; i < data.size (); ++i) {
			char ch = data[i];
			if (ch == '\\') {
				ch = data[++i];
				if (ch == 'x') {
					ret += (char) ((hex_char_to_dec (data[i + 1]) << 4) | hex_char_to_dec (data[i + 2]));
					i += 2;
				} else {
					ret += '\\';
					ret += ch;
				}
			} else {
				ret += ch;
			}
		}
		return ret;
	}
	static std::string escape_u_str_encode (std::string_view data) {
		const static char *hex_char = "0123456789ABCDEF";
		std::string ret = "";
		for (size_t i = 0; i < data.size (); ++i) {
			char ch = data[i];
			if (ch & 0x80) {
				ret += "\\u";
				ret += hex_char[((unsigned char) ch) >> 4];
				ret += hex_char[((unsigned char) ch) % 16];
				ch = data[++i];
				ret += hex_char[((unsigned char) ch) >> 4];
				ret += hex_char[((unsigned char) ch) % 16];
			} else {
				ret += ch;
			}
		}
		return ret;
	}
	static std::string escape_u_str_decode (std::string_view data) {
		std::string ret = "";
		for (size_t i = 0; i < data.size (); ++i) {
			char ch = data[i];
			if (ch == '\\') {
				ch = data[++i];
				if (ch == 'u') {
					ret += (char) ((hex_char_to_dec (data[i + 1]) << 4) | hex_char_to_dec (data[i + 2]));
					ret += (char) ((hex_char_to_dec (data[i + 3]) << 4) | hex_char_to_dec (data[i + 4]));
					i += 4;
				} else {
					ret += '\\';
					ret += ch;
				}
			} else {
				ret += ch;
			}
		}
		return ret;
	}
	static std::string base64_encode (std::string_view data) {
		std::string ret;
		int i = 0, j = 0;
		unsigned char char_3[3], char_4[4];
		unsigned int in_len = data.size ();
		unsigned char* bytes_to_encode = (unsigned char*) &data[0];
		while (in_len--) {
			char_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_4[0] = (char_3[0] & 0xfc) >> 2;
				char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
				char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
				char_4[3] = char_3[2] & 0x3f;

				for (i = 0; i < 4; i++)
					ret += base64_chars[char_4[i]];
				i = 0;
			}
		}
		if (i) {
			for (j = i; j < 3; j++)
				char_3[j] = '\0';
			char_4[0] = (char_3[0] & 0xfc) >> 2;
			char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
			char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
			for (j = 0; j < i + 1; j++)
				ret += base64_chars[char_4[j]];
			while ((i++ < 3))
				ret += '=';
		}
		return ret;
	}
	static std::string base64_decode (std::string_view data) {
		int in_len = data.size (), i = 0, j = 0, in_ = 0;
		unsigned char char_4[4], char_3[3];
		std::string ret;
		auto is_base64 = [] (unsigned char c) { return (isalnum (c) || (c == '+') || (c == '/')); };
		while (in_len-- && (data[in_] != '=') && is_base64 (data[in_])) {
			char_4[i++] = data[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_4[i] = (unsigned char) base64_chars.find (char_4[i]);
				char_3[0] = (char_4[0] << 2) + ((char_4[1] & 0x30) >> 4);
				char_3[1] = ((char_4[1] & 0xf) << 4) + ((char_4[2] & 0x3c) >> 2);
				char_3[2] = ((char_4[2] & 0x3) << 6) + char_4[3];

				for (i = 0; i < 3; i++)
					ret += char_3[i];
				i = 0;
			}
		}
		if (i) {
			for (j = 0; j < i; j++)
				char_4[j] = (unsigned char) base64_chars.find (char_4[j]);
			char_3[0] = (char_4[0] << 2) + ((char_4[1] & 0x30) >> 4);
			char_3[1] = ((char_4[1] & 0xf) << 4) + ((char_4[2] & 0x3c) >> 2);
			for (j = 0; j < i - 1; j++) ret += char_3[j];
		}
		return ret;
	}
};

__declspec (selectany) const std::string tool_Encoding::base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#endif //__TOOL_ENCODING_HPP__
