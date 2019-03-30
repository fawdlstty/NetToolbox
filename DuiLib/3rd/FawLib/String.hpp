////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  Encoding
// Description: 编码转换类
// Class URI:   https://github.com/fawdlstty/FawLib
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     MIT
// Last Update: Jan 24, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FAWLIB__STRING_HPP__
#define FAWLIB__STRING_HPP__



#include <iostream>
#include <initializer_list>
//#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <tuple>
#include <map>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <tchar.h>

#include "Encoding.hpp"



namespace faw {
	enum class flag {
		none			= 0x00000000,
		decimal_2		= 0x00000000,
		decimal_3		= 0x00010000,
		decimal_4		= 0x00020000,
		decimal_5		= 0x00030000,
		decimal_6		= 0x00040000,
		decimal_7		= 0x00050000,
		decimal_8		= 0x00060000,
		decimal_9		= 0x00070000,
		whole_10		= 0x00080000,
		whole_100		= 0x00090000,
		whole_1k		= 0x000a0000,
		whole_10k		= 0x000b0000,
		whole_100k		= 0x000c0000,
		whole_1m		= 0x000d0000,
		decimal_0		= 0x000e0000,
		decimal_1		= 0x000f0000,
		_align			= 0x00300000,
		align_left		= 0x00100000,
		align_middle	= 0x00200000,
		align_right		= 0x00000000,
	};
	// 用于将数据输入管道时，设置左右两侧padding以及浮点数小数位数设置
	inline flag set_width (size_t _width, flag _f = flag::none) { return (flag) ((_width & 0xffff) | (size_t) _f); }

	class String {
	public:
		// 字符串构造函数
		String () {}
		String (const char _c, size_t _len = 1) { m_str.assign (_len, (TCHAR) _c); }
		String (const wchar_t _c, size_t _len = 1) { m_str.assign (_len, (TCHAR) _c); }
		String (const char *_s, size_t _len = _npos) { m_str = Encoding::gb18030_to_T (_len == _npos ? std::string_view (_s) : std::string_view (_s, _len)); }
		String (const wchar_t *_s, size_t _len = _npos) { m_str = Encoding::utf16_to_T (_len == _npos ? std::wstring_view (_s) : std::wstring_view (_s, _len)); }
		String (const std::string &_s): m_str (Encoding::gb18030_to_T (std::string_view (_s))) {}
		String (const std::wstring &_s): m_str (Encoding::utf16_to_T (std::wstring_view (_s))) {}
#if _HAS_CXX17
		String (const std::string_view _s): m_str (Encoding::gb18030_to_T (_s)) {}
		String (const std::wstring_view _s): m_str (Encoding::utf16_to_T (_s)) {}
#endif
		String (const String &_s): m_str (_s.m_str) {}
		String (const String *_s): m_str (_s->m_str) {}

		// 字符串符号重载函数
		String &operator= (const char *_s) { m_str = Encoding::gb18030_to_T (std::string_view (_s)); return *this; }
		String &operator= (const wchar_t *_s) { m_str = Encoding::utf16_to_T (std::wstring_view (_s)); return *this; }
		String &operator= (const std::string &_s) { m_str = Encoding::gb18030_to_T (std::string_view (_s)); return *this; }
		String &operator= (const std::wstring &_s) { m_str = Encoding::utf16_to_T (std::wstring_view (_s)); return *this; }
		String &operator= (const String &_s) { m_str = _s.m_str; return *this; }
		String &operator= (const String *_s) { m_str = _s->m_str; return *this; }
#if _HAS_CXX17
		String &operator= (const std::string_view _s) { m_str = Encoding::gb18030_to_T (_s); return *this; }
		String &operator= (const std::wstring_view _s) { m_str = Encoding::utf16_to_T (_s); return *this; }
#endif
		//
		String &operator+= (const char _c) { m_str += (TCHAR) _c; return *this; }
		String &operator+= (const wchar_t _c) { m_str += (TCHAR) _c; return *this; }
		String &operator+= (const char *_s) { m_str += Encoding::gb18030_to_T (std::string_view (_s)); return *this; }
		String &operator+= (const wchar_t *_s) { m_str += Encoding::utf16_to_T (std::wstring_view (_s)); return *this; }
		String &operator+= (const std::string &_s) { m_str += Encoding::gb18030_to_T (std::string_view (_s)); return *this; }
		String &operator+= (const std::wstring &_s) { m_str += Encoding::utf16_to_T (std::wstring_view (_s)); return *this; }
		String &operator+= (const String &_s) { m_str += _s.m_str; return *this; }
		String &operator+= (const String *_s) { m_str += _s->m_str; return *this; }
#if _HAS_CXX17
		String &operator+= (const std::string_view _s) { m_str += Encoding::gb18030_to_T (_s); return *this; }
		String &operator+= (const std::wstring_view _s) { m_str += Encoding::utf16_to_T (_s); return *this; }
#endif
		//
		String operator+ (const char _c) { String _o (this); _o.m_str += (TCHAR) _c; return _o; }
		String operator+ (const wchar_t _c) { String _o (this); _o.m_str += (TCHAR) _c; return _o; }
		String operator+ (const char *_s) { String _o (this); _o.m_str += Encoding::gb18030_to_T (std::string_view (_s)); return _o; }
		String operator+ (const wchar_t *_s) { String _o (this); _o.m_str += Encoding::utf16_to_T (std::wstring_view (_s)); return _o; }
		String operator+ (const std::string &_s) { String _o (this); _o.m_str += Encoding::gb18030_to_T (std::string_view (_s)); return _o; }
		String operator+ (const std::wstring &_s) { String _o (this); _o.m_str += Encoding::utf16_to_T (std::wstring_view (_s)); return _o; }
		String operator+ (const String &_s) { String _o (this); _o.m_str += _s.m_str; return _o; }
		String operator+ (const String *_s) { String _o (this); _o.m_str += _s->m_str; return _o; }
#if _HAS_CXX17
		String operator+ (const std::string_view _s) { String _o (this); _o.m_str += Encoding::gb18030_to_T (_s); return _o; }
		String operator+ (const std::wstring_view _s) { String _o (this); _o.m_str += Encoding::utf16_to_T (_s); return _o; }
#endif
		//
		friend String operator+ (const char _c, String &_o) { _o.m_str.insert (_o.m_str.begin (), (TCHAR) _c); }
		friend String operator+ (const wchar_t _c, String &_o) { _o.m_str.insert (_o.m_str.begin (), (TCHAR) _c); }
		friend String operator+ (const char *_s, String &_o) { String _so (_s); _so.m_str += _o.m_str; return _so; }
		friend String operator+ (const wchar_t *_s, String &_o) { String _so (_s); _so.m_str += _o.m_str; return _so; }
		friend String operator+ (const std::string &_s, String &_o) { String _so (_s); _so.m_str += _o.m_str; return _so; }
		friend String operator+ (const std::wstring &_s, String &_o) { String _so (_s); _so.m_str += _o.m_str; return _so; }
#if _HAS_CXX17
		friend String operator+ (const std::string_view &_s, String &_o) { String _so (_s); _so.m_str += _o.m_str; return _so; }
		friend String operator+ (const std::wstring_view &_s, String &_o) { String _so (_s); _so.m_str += _o.m_str; return _so; }
#endif

		// 比较
		bool operator== (const char *_s) const { String _o (_s); return m_str == _o.m_str; }
		bool operator== (const wchar_t *_s) const { String _o (_s); return m_str == _o.m_str; }
		bool operator== (const std::string &_s) const { String _o (_s); return m_str == _o.m_str; }
		bool operator== (const std::wstring &_s) const { String _o (_s); return m_str == _o.m_str; }
		bool operator== (const String &_o) const { return m_str == _o.m_str; }
		bool operator== (const String *_o) const { return m_str == _o->m_str; }
#if _HAS_CXX17
		bool operator== (const std::string_view _s) const { String _o (_s); return m_str == _o.m_str; }
		bool operator== (const std::wstring_view _s) const { String _o (_s); return m_str == _o.m_str; }
#endif
		//
		friend bool operator== (const char *_s, String &_o) { return _o == _s; }
		friend bool operator== (const wchar_t *_s, String &_o) { return _o == _s; }
		friend bool operator== (const std::string &_s, String &_o) { return _o == _s; }
		friend bool operator== (const std::wstring &_s, String &_o) { return _o == _s; }
		friend bool operator== (const String *_o, const String &_o2) { return _o2  == *_o; }
#if _HAS_CXX17
		friend bool operator== (const std::string_view _s, String &_o) { return _o == _s; }
		friend bool operator== (const std::wstring_view _s, String &_o) { return _o == _s; }
#endif
		//
		bool operator!= (const char *_s) const { String _o (_s); return m_str != _o.m_str; }
		bool operator!= (const wchar_t *_s) const { String _o (_s); return m_str != _o.m_str; }
		bool operator!= (const std::string &_s) const { String _o (_s); return m_str != _o.m_str; }
		bool operator!= (const std::wstring &_s) const { String _o (_s); return m_str != _o.m_str; }
		bool operator!= (const String &_o) const { return m_str != _o.m_str; }
		bool operator!= (const String *_o) const { return m_str != _o->m_str; }
#if _HAS_CXX17
		bool operator!= (const std::string_view _s) const { String _o (_s); return m_str != _o.m_str; }
		bool operator!= (const std::wstring_view _s) const { String _o (_s); return m_str != _o.m_str; }
#endif
		//
		friend bool operator!= (const char *_s, String &_o) { return _o != _s; }
		friend bool operator!= (const wchar_t *_s, String &_o) { return _o != _s; }
		friend bool operator!= (const std::string &_s, String &_o) { return _o != _s; }
		friend bool operator!= (const std::wstring &_s, String &_o) { return _o != _s; }
		friend bool operator!= (String *_o, String &_o2) { return _o2  != *_o; }
#if _HAS_CXX17
		friend bool operator!= (const std::string_view _s, String &_o) { return _o != _s; }
		friend bool operator!= (const std::wstring_view _s, String &_o) { return _o != _s; }
#endif
		//
		bool is_equal (const char *_s) { return operator== (_s); }
		bool is_equal (const wchar_t *_s) { return operator== (_s); }
		bool is_equal (const std::string &_s) { return operator== (_s); }
		bool is_equal (const std::wstring &_s) { return operator== (_s); }
		bool is_equal (String &_o) { return operator== (_o); }
		bool is_equal (String *_o) { return operator== (_o); }
#if _HAS_CXX17
		bool is_equal (const std::string_view _s) { return operator== (_s); }
		bool is_equal (const std::wstring_view _s) { return operator== (_s); }
#endif
		//
		bool is_equal_nocase (const char *_s) { String _o (_s); return is_equal_nocase (_o); }
		bool is_equal_nocase (const wchar_t *_s) { String _o (_s); return is_equal_nocase (_o); }
		bool is_equal_nocase (const std::string &_s) { String _o (_s); return is_equal_nocase (_o); }
		bool is_equal_nocase (const std::wstring &_s) { String _o (_s); return is_equal_nocase (_o); }
		bool is_equal_nocase (String &_o) { size_t size = m_str.size ();
			if (size != _o.m_str.size ())
				return false;
			for (size_t i = 0; i < size; ++i) {
				if (m_str [i] == _o.m_str [i]) {
					continue;
				} else if (m_str [i] >= _T ('a') && m_str [i] <= _T ('z')) {
					if (m_str [i] - _T ('a') + _T ('A') != _o.m_str [i])
						return false;
				} else if (m_str [i] >= _T ('A') && m_str [i] <= _T ('Z')) {
					if (m_str [i] - _T ('A') + _T ('a') != _o.m_str [i])
						return false;
				} else {
					return false;
				}
			}
			return true; }
		bool is_equal_nocase (String *_o) { return is_equal_nocase (*_o); }
#if _HAS_CXX17
		bool is_equal_nocase (const std::string_view _s) { String _o (_s); return is_equal_nocase (_o); }
		bool is_equal_nocase (const std::wstring_view _s) { String _o (_s); return is_equal_nocase (_o); }
#endif

		// 其他符号重载函数
		String operator* (size_t n) {
			String _s;
			if (n == 0 || n == _npos)
				return _s;
			while (n-- > 0)
				_s.m_str += m_str;
			return _s;
		}
		friend String operator* (size_t n, String &_o) { return _o.operator* (n); }
		String &operator*= (size_t n) {
			if (n == 0 || n == _npos) {
				m_str.clear ();
			} else {
				string_t _str = m_str;
				while (--n > 0)
					m_str += _str;
			}
			return *this;
		}

		// 字符串迭代器处理
		string_t::iterator begin () { return m_str.begin (); }
		string_t::iterator end () { return m_str.end (); }
		string_t::const_iterator cbegin () { return m_str.cbegin (); }
		string_t::const_iterator cend () { return m_str.cend (); }
		string_t::reverse_iterator rbegin () { return m_str.rbegin (); }
		string_t::reverse_iterator rend () { return m_str.rend (); }
		string_t::const_reverse_iterator crbegin () { return m_str.crbegin (); }
		string_t::const_reverse_iterator crend () { return m_str.crend (); }

		// 字符串流处理
		String &operator<< (flag _f) { m_last_flag = _f; return *this; }
		friend String &operator>> (flag _f, String &_o) { _o.m_last_flag = _f; return _o; }
		String &operator>> (TCHAR &n) {
			if (m_str.empty ()) {
				n = _T ('\0');
			} else {
				n = *m_str.rbegin ();
				m_str.pop_back ();
			}
			return *this;
		}
		String &operator<< (TCHAR n) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			string_t _str (1, n);
			_make_flag_space (_width, _align, _str);
			m_str += _str;
			return *this;
		}
		friend String &operator>> (TCHAR n, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			string_t _str (1, n);
			_make_flag_space (_width, _align, _str);
			_o.m_str.insert (_o.m_str.begin (), _str.begin (), _str.end ());
			return _o;
		}
		friend String &operator<< (TCHAR &n, String &_o) {
			if (_o.m_str.empty ()) {
				n = _T ('\0');
			} else {
				n = *_o.m_str.begin ();
				_o.m_str.erase (_o.m_str.begin ());
			} return _o;
		}
		String &operator>> (int &n) {
			n = 0;
			int _level = 1;
			for (size_t i = m_str.size () - 1; i != _npos; --i) {
				if (m_str[i] >= _T ('0') && m_str[i] <= _T ('9')) {
					n += _level * (m_str[i] - _T ('0'));
					m_str.erase (m_str.begin () + i);
					_level *= 10;
				} else if (m_str[i] == _T ('-')) {
					n = 0 - n;
					m_str.erase (m_str.begin () + i);
					break;
				} else {
					break;
				}
			}
			return *this;
		}
		String &operator<< (int n) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _tmp = String::format (_T ("%d"), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			m_str += _tmp.m_str;
			return *this;
		}
		friend String &operator>> (int n, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _tmp = String::format (_T ("%d"), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			_o.m_str.insert (_o.m_str.begin (), _tmp.m_str.begin (), _tmp.m_str.end ());
			return _o;
		}
		friend String &operator<< (int &n, String &_o) {
			n = 0;
			if (_o.m_str.size () == 0)
				return _o;
			bool _sign = (_o.m_str[0] == _T ('-'));
			size_t i = (_sign ? 1 : 0);
			for (; i < _o.m_str.size () && _o.m_str[i] >= _T ('0') && _o.m_str[i] <= _T ('9'); ++i)
				n = n * 10 + (_o.m_str[i] - _T ('0'));
			if (_sign)
				n = 0 - n;
			_o.m_str.erase (_o.m_str.begin (), _o.m_str.begin () + i);
			return _o;
		}
		String &operator>> (unsigned int &n) {
			n = 0;
			unsigned int _level = 1;
			for (size_t i = m_str.size () - 1; i != _npos; --i) {
				if (m_str[i] >= _T ('0') && m_str[i] <= _T ('9')) {
					n += _level * (m_str[i] - _T ('0'));
					m_str.erase (m_str.begin () + i);
					_level *= 10;
				} else {
					break;
				}
			}
			return *this;
		}
		String &operator<< (unsigned int n) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _tmp = String::format (_T ("%u"), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			m_str += _tmp.m_str;
			return *this;
		}
		friend String &operator>> (unsigned int n, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _tmp = String::format (_T ("%u"), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			_o.m_str.insert (_o.m_str.begin (), _tmp.m_str.begin (), _tmp.m_str.end ());
			return _o;
		}
		friend String &operator<< (unsigned int &n, String &_o) {
			n = 0;
			if (_o.m_str.size () == 0)
				return _o;
			size_t i = 0;
			for (; i < _o.m_str.size () && _o.m_str[i] >= _T ('0') && _o.m_str[i] <= _T ('9'); ++i)
				n = n * 10 + (_o.m_str[i] - _T ('0'));
			_o.m_str.erase (_o.m_str.begin (), _o.m_str.begin () + i);
			return _o;
		}
		String &operator>> (float &n) {
			n = 0;
			float _level = 1;
			for (size_t i = m_str.size () - 1; i != _npos; --i) {
				if (m_str [i] >= _T ('0') && m_str [i] <= _T ('9')) {
					n += _level * (m_str [i] - _T ('0'));
					m_str.erase (m_str.begin () + i);
					_level *= 10;
				} else if (m_str [i] == _T ('-')) {
					n = 0 - n;
					m_str.erase (m_str.begin () + i);
					break;
				} else if (m_str [i] == _T ('.')) {
					n /= (_level / 10);
					_level = 0.1f;
				} else {
					break;
				}
			}
			return *this;
		}
		String &operator<< (float n) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _tmp = String::format (String::format (_T ("%%.%df"), _dot).c_str (), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			m_str += _tmp.m_str;
			return *this;
		}
		friend String &operator>> (float n, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _tmp = String::format (String::format (_T ("%%.%df"), _dot).c_str (), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			_o.m_str.insert (_o.m_str.begin (), _tmp.m_str.begin (), _tmp.m_str.end ());
			return _o;
		}
		friend String &operator<< (float &n, String &_o) {
			n = 0;
			if (_o.m_str.size () == 0)
				return _o;
			bool _sign = (_o.m_str [0] == _T ('-'));
			float _level = 1.0f;
			size_t i = (_sign ? 1 : 0);
			for (; i < _o.m_str.size (); ++i) {
				if (_o.m_str [i] >= _T ('0') && _o.m_str [i] <= _T ('9')) {
					if (_level >= 1.0f) {
						n = n * 10 + (_o.m_str [i] - _T ('0'));
					} else {
						n += _level * (_o.m_str [i] - _T ('0'));
						_level /= 10;
					}
				} else if (_o.m_str [i] == _T ('.')) {
					_level = 0.1f;
				}
			}
			if (_sign)
				n = 0 - n;
			_o.m_str.erase (_o.m_str.begin (), _o.m_str.begin () + i);
			return _o;
		}
		String &operator>> (double &n) {
			n = 0;
			double _level = 1;
			for (size_t i = m_str.size () - 1; i != _npos; --i) {
				if (m_str [i] >= _T ('0') && m_str [i] <= _T ('9')) {
					n += _level * (m_str [i] - _T ('0'));
					m_str.erase (m_str.begin () + i);
					_level *= 10;
				} else if (m_str [i] == _T ('-')) {
					n = 0 - n;
					m_str.erase (m_str.begin () + i);
					break;
				} else if (m_str [i] == _T ('.')) {
					n /= (_level / 10);
					_level = 0.1;
				} else {
					break;
				}
			}
			return *this;
		}
		String &operator<< (double n) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _tmp = String::format (String::format (_T ("%%.%dlf"), _dot).c_str (), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			m_str += _tmp.m_str;
			return *this;
		}
		friend String &operator>> (double n, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _tmp = String::format (String::format (_T ("%%.%dlf"), _dot).c_str (), n);
			_make_flag_space (_width, _align, _tmp.m_str);
			_o.m_str.insert (_o.m_str.begin (), _tmp.m_str.begin (), _tmp.m_str.end ());
			return _o;
		}
		friend String &operator<< (double &n, String &_o) {
			n = 0;
			if (_o.m_str.size () == 0)
				return _o;
			bool _sign = (_o.m_str [0] == _T ('-'));
			double _level = 1.0f;
			size_t i = (_sign ? 1 : 0);
			for (; i < _o.m_str.size (); ++i) {
				if (_o.m_str [i] >= _T ('0') && _o.m_str [i] <= _T ('9')) {
					if (_level >= 1.0f) {
						n = n * 10 + (_o.m_str [i] - _T ('0'));
					} else {
						n += _level * (_o.m_str [i] - _T ('0'));
						_level /= 10;
					}
				} else if (_o.m_str [i] == _T ('.')) {
					_level = 0.1f;
				}
			}
			if (_sign)
				n = 0 - n;
			_o.m_str.erase (_o.m_str.begin (), _o.m_str.begin () + i);
			return _o;
		}
		friend std::ostream &operator<< (std::ostream &_stm, String &_s) { _stm << _s.stra (); return _stm; }
		friend std::wostream &operator<< (std::wostream &_stm, String &_s) { _stm << _s.strw (); return _stm; }
		String &operator<< (const char *_s) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			m_str += _s2.m_str;
			return *this;
		}
		friend String &operator>> (const char *_s, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			_o.m_str.insert (_o.m_str.begin (), _s2.m_str.begin (), _s2.m_str.end ());
			return _o;
		}
		String &operator<< (const wchar_t *_s) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			m_str += _s2.m_str;
			return *this;
		}
		friend String &operator>> (const wchar_t *_s, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			_o.m_str.insert (_o.m_str.begin (), _s2.m_str.begin (), _s2.m_str.end ());
			return _o;
		}
		String &operator<< (std::string &_s) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			m_str += _s2.m_str;
			return *this;
		}
		friend String &operator>> (std::string &_s, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			_o.m_str.insert (_o.m_str.begin (), _s2.m_str.begin (), _s2.m_str.end ());
			return _o;
		}
		String &operator<< (std::wstring &_s) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			m_str += _s2.m_str;
			return *this;
		}
		friend String &operator>> (std::wstring &_s, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			_o.m_str.insert (_o.m_str.begin (), _s2.m_str.begin (), _s2.m_str.end ());
			return _o;
		}
#if _HAS_CXX17
		String &operator<< (std::string_view &_s) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			m_str += _s2.m_str;
			return *this;
		}
		friend String &operator>> (std::string_view &_s, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			_o.m_str.insert (_o.m_str.begin (), _s2.m_str.begin (), _s2.m_str.end ());
			return _o;
		}
		String &operator<< (std::wstring_view &_s) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			m_str += _s2.m_str;
			return *this;
		}
		friend String &operator>> (std::wstring_view &_s, String &_o) {
			double _dot = 0;
			size_t _width = 0;
			flag _align = flag::align_right;
			std::tie (_dot, _width, _align) = _get_flag_info (_o.m_last_flag);
			//
			String _s2 (_s);
			_make_flag_space (_width, _align, _s2.m_str);
			_o.m_str.insert (_o.m_str.begin (), _s2.m_str.begin (), _s2.m_str.end ());
			return _o;
		}
#endif

		// 字符串查找
		size_t find (TCHAR _ch, size_t _off = 0) { return m_str.find (_ch, _off); }
		size_t rfind (TCHAR _ch, size_t _off = _npos) { return m_str.rfind (_ch, _off); }
#ifdef _UNICODE
		size_t find (std::string &_s, size_t _off = 0) { return m_str.find (Encoding::gb18030_to_utf16 (_s), _off); }
		size_t find (std::wstring &_s, size_t _off = 0) { return m_str.find (_s, _off); }
		size_t rfind (std::string &_s, size_t _off = 0) { return m_str.rfind (Encoding::gb18030_to_utf16 (_s), _off); }
		size_t rfind (std::wstring &_s, size_t _off = 0) { return m_str.rfind (_s, _off); }
#	if _HAS_CXX17
		size_t find (std::string_view _s, size_t _off = 0) { return m_str.find (Encoding::gb18030_to_utf16 (_s), _off); }
		size_t find (std::wstring_view _s, size_t _off = 0) { return m_str.find (_s, _off); }
		size_t rfind (std::string_view _s, size_t _off = 0) { return m_str.rfind (Encoding::gb18030_to_utf16 (_s), _off); }
		size_t rfind (std::wstring_view _s, size_t _off = 0) { return m_str.rfind (_s, _off); }
#	endif
#else
		size_t find (std::string &_s, size_t _off = 0) { return m_str.find (_s, _off); }
		size_t find (std::wstring &_s, size_t _off = 0) { return m_str.find (Encoding::utf16_to_gb18030 (_s), _off); }
		size_t rfind (std::string &_s, size_t _off = 0) { return m_str.rfind (_s, _off); }
		size_t rfind (std::wstring &_s, size_t _off = 0) { return m_str.rfind (Encoding::utf16_to_gb18030 (_s), _off); }
#	if _HAS_CXX17
		size_t find (std::string_view _s, size_t _off = 0) { return m_str.find (_s, _off); }
		size_t find (std::wstring_view _s, size_t _off = 0) { return m_str.find (Encoding::utf16_to_gb18030 (_s), _off); }
		size_t rfind (std::string_view _s, size_t _off = 0) { return m_str.rfind (_s, _off); }
		size_t rfind (std::wstring_view _s, size_t _off = 0) { return m_str.rfind (Encoding::utf16_to_gb18030 (_s), _off); }
#	endif
#endif
		size_t find_any (std::initializer_list<TCHAR> _cs) {
			size_t _ret = _npos;
			for (auto &_c : _cs) {
				size_t _n = find (_c);
				_ret = (_ret == _npos || (_n != _npos && _n < _ret) ? _n : _ret);
			}
			return _ret;
		}
		size_t find_any (std::initializer_list<std::string> _ss) {
			size_t _ret = _npos;
			for (auto &_s : _ss) {
				size_t _n = find (std::string_view (_s));
				_ret = (_ret == _npos || (_n != _npos && _n < _ret) ? _n : _ret);
			}
			return _ret;
		}
		size_t find_any (std::initializer_list<std::wstring> _ss) {
			size_t _ret = _npos;
			for (auto &_s : _ss) {
				size_t _n = find (std::wstring_view (_s));
				_ret = (_ret == _npos || (_n != _npos && _n < _ret) ? _n : _ret);
			}
			return _ret;
		}
		size_t rfind_any (std::initializer_list<TCHAR> _cs) {
			size_t _ret = _npos;
			for (auto &_c : _cs) {
				size_t _n = rfind (_c);
				_ret = (_ret == _npos || (_n != _npos && _n > _ret) ? _n : _ret);
			}
			return _ret;
		}
		size_t rfind_any (std::initializer_list<std::string> _ss) {
			size_t _ret = _npos;
			for (auto &_s : _ss) {
				size_t _n = rfind (std::string_view (_s));
				_ret = (_ret == _npos || (_n != _npos && _n > _ret) ? _n : _ret);
			}
			return _ret;
		}
		size_t rfind_any (std::initializer_list<std::wstring> _ss) {
			size_t _ret = _npos;
			for (auto &_s : _ss) {
				size_t _n = rfind (std::wstring_view (_s));
				_ret = (_ret == _npos || (_n != _npos && _n > _ret) ? _n : _ret);
			}
			return _ret;
		}

		// 字符串处理（生成新字符串）
		String trim_left () const { String _s (this); _s.m_str.erase (0, _s.m_str.find_first_not_of (_T (' '))); return _s; }
		String trim_right () const { String _s (this); _s.m_str.erase (_s.m_str.find_last_not_of (' ') + 1); return _s; }
		String trim () const { return trim_left ().trim_right_self (); }
		String upper () const { String _s (this); std::transform (_s.m_str.begin (), _s.m_str.end (), _s.m_str.begin (), ::toupper); return _s; }
		String lower () const { String _s (this); std::transform (_s.m_str.begin (), _s.m_str.end (), _s.m_str.begin (), ::tolower); return _s; }
		String reverse () const {
			String _s (this);
			size_t size = _s.m_str.size ();
			for (size_t i = 0; i < size / 2; ++i) {
				TCHAR ch = _s.m_str [i];
				_s.m_str [i] = _s.m_str [size - i - 1];
				_s.m_str [size - i - 1] = ch;
			}
			return _s;
		}
		String replace (std::string_view _src, std::string_view _dest) const {
			String _s (this);
#ifdef _UNICODE
			std::wstring _tmp_src = faw::Encoding::gb18030_to_utf16 (_src);
			std::wstring _tmp_dest = faw::Encoding::gb18030_to_utf16 (_dest);
			return replace (_tmp_src, _tmp_dest);
#else
			size_t pos = _s.m_str.find (_src);
			while (pos != _npos) {
				_s.m_str.replace (pos, _src.size (), _dest);
				pos = _src.find (_src, pos + 2);
			}
			return _s;
#endif
		}
		String replace (std::wstring_view _src, std::wstring_view _dest) const {
			String _s (this);
#ifdef _UNICODE
			size_t pos = _s.m_str.find (_src);
			while (pos != _npos) {
				_s.m_str.replace (pos, _src.size (), _dest);
				pos = _src.find (_src, pos + 2);
			}
			return _s;
#else
			std::string _tmp_src = Encoding::utf16_to_gb18030 (_src);
			std::string _tmp_dest = Encoding::utf16_to_gb18030 (_dest);
			return replace (_tmp_src, _tmp_dest);
#endif
		}
		String replace (TCHAR _src, TCHAR _dest) const {
			String _s (this);
			size_t pos = _s.m_str.find (_src);
			while (pos != _npos) {
				_s.m_str [pos] = _dest;
				pos = _s.m_str.find (_src, pos + 1);
			}
			return _s;
		}
		String left (size_t n) const { return (n >= size () ? str () : str ().substr (0, n)); }
		String right (size_t n) const { return (n >= size () ? str () : str ().substr (size () - n)); }
		String substr (size_t begin, size_t len = String::_npos) const { return (begin >= size () ? _T ("") : (begin + len >= size () ? str ().substr (begin) : str ().substr (begin, len))); }

		// 字符串处理（改变自身）
		String &trim_left_self () { m_str.erase (0, m_str.find_first_not_of (_T (' '))); return *this; }
		String &trim_right_self () { m_str.erase (m_str.find_last_not_of (' ') + 1); return *this; }
		String &trim_self () { return trim_left_self ().trim_right_self (); }
		String &upper_self () { std::transform (m_str.begin (), m_str.end (), m_str.begin (), ::toupper); return *this; }
		String &lower_self () { std::transform (m_str.begin (), m_str.end (), m_str.begin (), ::tolower); return *this; }
		String &reverse_self () {
			size_t size = m_str.size ();
			for (size_t i = 0; i < size / 2; ++i) {
				TCHAR ch = m_str [i];
				m_str [i] = m_str [size - i - 1];
				m_str [size - i - 1] = ch;
			}
			return *this;
		}
		String &replace_self (std::string_view _src, std::string_view _dest) {
#ifdef _UNICODE
			std::wstring _tmp_src = faw::Encoding::gb18030_to_utf16 (_src);
			std::wstring _tmp_dest = faw::Encoding::gb18030_to_utf16 (_dest);
			return replace_self (_tmp_src, _tmp_dest);
#else
			size_t pos = m_str.find (_src);
			while (pos != _npos) {
				m_str.replace (pos, _src.size (), _dest);
				pos = m_str.find (_src, pos + 2);
			}
			return *this;
#endif
		}
		String &replace_self (std::wstring_view _src, std::wstring_view _dest) {
#ifdef _UNICODE
			size_t pos = m_str.find (_src);
			while (pos != _npos) {
				m_str.replace (pos, _src.size (), _dest);
				pos = m_str.find (_src, pos + 2);
			}
			return *this;
#else
			std::string _tmp_src = Encoding::utf16_to_gb18030 (_src);
			std::string _tmp_dest = Encoding::utf16_to_gb18030 (_dest);
			return replace_self (_tmp_src, _tmp_dest);
#endif
		}
		String &replace_self (TCHAR _src, TCHAR _dest) {
			size_t pos = m_str.find (_src);
			while (pos != _npos) {
				m_str [pos] = _dest;
				pos = m_str.find (_src);
			}
			return *this;
		}
		String &left_self (size_t n) { m_str = (n >= size () ? str () : str ().substr (0, n)); return *this; }
		String &right_self (size_t n) { m_str = (n >= size () ? str () : str ().substr (size () - n)); return *this; }
		String &substr_self (size_t begin, size_t len = String::_npos) { m_str = (begin >= size () ? _T ("") : (begin + len >= size () ? str ().substr (begin) : str ().substr (begin, len))); return *this; }

		// 字符串基本方法
		bool empty () const { return m_str.empty (); }
		void clear () { m_str.clear (); }
		void free () { string_t _str = _T (""); _str.swap (m_str); }
		size_t size () const { return m_str.size (); }
		//TCHAR &operator[] (intptr_t n) { n = (n > 0 ? (n < size () ? n : size () - 1) : (n > -size () ? size () - n : 0)); return m_str [n]; }
		TCHAR &operator[] (size_t n) { return m_str [n]; }
		TCHAR &at (intptr_t n) { static TCHAR _ch = _T ('\0'); return (size () == 0 ? _ch : m_str [((n % size ()) + size ()) % size ()]); }
		const TCHAR *c_str () const { return m_str.c_str (); }
		const string_t &str () const { return m_str; }
#if _HAS_CXX17
		string_view_t str_view () const { return string_view_t (m_str); }
#endif
		std::string stra () const { return Encoding::T_to_gb18030 (m_str); }
		std::wstring strw () const { return Encoding::T_to_utf16 (m_str); }
		//const TCHAR *operator() () { return m_str.c_str (); }

		// 比较及hash
		bool operator< (const String &_str) const { return m_str < _str.m_str; }
		bool operator<= (const String &_str) const { return m_str <= _str.m_str; }
		bool operator> (const String &_str) const { return m_str > _str.m_str; }
		bool operator>= (const String &_str) const { return m_str >= _str.m_str; }

		//
		std::vector<String> split (TCHAR _sp = _T (' '), bool no_empty = true) {
			size_t start = 0, start_find = 0;
			std::vector<String> v;
			while (start_find < m_str.size ()) {
				size_t p = m_str.find (_sp, start_find);
				if (p == _npos) p = m_str.size ();
				if (!no_empty || p > start)
					v.push_back (m_str.substr (start, p - start));
				start = start_find = p + 1;
			}
			return v;
		}
		std::vector<String> split (string_view_t _sp = _T (" "), bool no_empty = true) {
			size_t start = 0, start_find = 0;
			std::vector<String> v;
			while (start_find < m_str.size ()) {
				size_t p = m_str.find (_sp, start_find);
				if (p == _npos) p = m_str.size ();
				if (!no_empty || p > start)
					v.push_back (m_str.substr (start, p - start));
				start = start_find = p + _sp.size ();
			}
			return v;
		}
		std::vector<String> match_regex (std::string _re) {
			std::string _src = stra ();
			//std::string err = "";
			std::vector<String> v;
			try {
				std::regex r (_re);
				std::smatch m;
				std::string::const_iterator _begin = _src.cbegin (), _end = _src.cend ();
				while (std::regex_search (_begin, _end, m, r)) {
					v.push_back (m [0].str ());
					_begin = m [0].second;
				}
			//} catch (std::exception &e) {
			//	err = e.what ();
			} catch (...) {
			//	err = "未知错误。";
			}
			return v;
		}
		static String format (const char *_fmt, ...) {
			std::string str_result = "";
			if (!_fmt || !*_fmt)
				return str_result;
			try {
				va_list ap;
#ifndef __GNUC__
				//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				size_t final_n, n = lstrlenA (_fmt) * 2;
				std::unique_ptr<char []> formatted;
				while (true) {
					formatted.reset (new char [n]);
					va_start (ap, _fmt);
					final_n = _vsnprintf_s (&formatted [0], n, _TRUNCATE, _fmt, ap);
					va_end (ap);
					if (final_n < 0 || final_n >= n)
						n += abs ((int) (final_n - n + 1));
					else
						break;
				}
				str_result = formatted.get ();
#else //__GNUC__
				char *buf = nullptr;
				va_start (ap, _fmt);
				int iresult = vasprintf (&buf, _fmt, ap);
				va_end (ap);
				if (buf) {
					if (iresult >= 0) {
						iresult = strlen (buf);
						str_result.append (buf, iresult);
					}
					free (buf);
				}
#endif //__GNUC__
			} catch (...) {
			}
			return str_result;
		}
		static String format (const wchar_t *_fmt, ...) {
			std::wstring str_result = L"";
			if (!_fmt || !*_fmt)
				return str_result;
			try {
				va_list ap;
#ifndef __GNUC__
				//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				size_t final_n, n = lstrlenW (_fmt) * 2;
				std::unique_ptr<wchar_t []> formatted;
				while (true) {
					formatted.reset (new wchar_t [n]);
					va_start (ap, _fmt);
					final_n = _vsnwprintf_s (&formatted [0], n, _TRUNCATE, _fmt, ap);
					va_end (ap);
					if (final_n < 0 || final_n >= n)
						n += abs ((int) (final_n - n + 1));
					else
						break;
				}
				str_result = formatted.get ();
#else //__GNUC__
				wchar_t *buf = nullptr;
				va_start (ap, _fmt);
				int iresult = vswprintf (&buf, _fmt, ap);
				va_end (ap);
				if (buf) {
					if (iresult >= 0) {
						iresult = strlen (buf);
						str_result.append (buf, iresult);
					}
					free (buf);
				}
#endif //__GNUC__
			} catch (...) {
			}
			return str_result;
		}
		static size_t _npos;

	private:
		// flag专用，用于提取flag信息
		static std::tuple<double, size_t, flag> _get_flag_info (flag &_f) {
			static std::map<flag, double> m_dot { { flag::decimal_0, 0 }, { flag::decimal_1, 0.1 }, { flag::decimal_2, 0.01 }, { flag::decimal_3, 0.001 }, { flag::decimal_4, 0.0001 }, { flag::decimal_5, 0.00001 }, { flag::decimal_6, 0.000001 }, { flag::decimal_7, 0.0000001 }, { flag::decimal_8, 0.00000001 }, { flag::decimal_9, 0.000000001 }, { flag::whole_10, 10 }, { flag::whole_100, 100 }, { flag::whole_1k, 1000 }, { flag::whole_10k, 10000 }, { flag::whole_100k, 100000 }, { flag::whole_1m, 1000000 } };
			//
			double _dot = 0;
			if (m_dot.find ((flag) (((size_t) _f) & 0x000f0000)) != m_dot.end ())
				_dot = m_dot [(flag) (((size_t) _f) & 0x000f0000)];
			size_t _width = ((size_t) _f) & 0x0000ffff;
			flag _align = (flag) (((size_t) _f) & ((size_t) flag::_align));
			_align = (_align == flag::_align ? flag::align_right : _align);
			_f = flag::none;
			return std::make_tuple (_dot, _width, _align);
		}
		// flag专用，用于获取padding字符串
		static void _make_flag_space (size_t _width, flag _align, string_t &_src) {
			size_t _left_count = 0, _right_count = 0;
			if (_align == flag::align_right && _width > _src.size ()) {
				_left_count = _width - _src.size ();
			} else if (_align == flag::align_middle && _width > _src.size ()) {
				_left_count = (_width - _src.size ()) / 2;
			}
			if (_align == flag::align_left && _width > _src.size ()) {
				_right_count = _width - _src.size ();
			} else if (_align == flag::align_middle && _width > _src.size ()) {
				_right_count = _width - _src.size () - _left_count;
			}
			_src.insert (_src.begin (), _left_count, _T (' '));
			_src.insert (_src.end (), _right_count, _T (' '));
		}

		string_t m_str;
		flag m_last_flag = flag::none;
	};

	__declspec (selectany) size_t String::_npos = string_t::npos;
}

#if _HAS_CXX17
inline namespace _faw {
	[[nodiscard]] inline ::faw::String operator "" _fs (const char *_s, size_t _len) noexcept { return ::faw::String (_s, (size_t) _len); }
	[[nodiscard]] inline ::faw::String operator "" _fs (const wchar_t *_s, size_t _len) noexcept { return ::faw::String (_s, (size_t) _len); }
}
#endif



#endif //FAWLIB__STRING_HPP__
