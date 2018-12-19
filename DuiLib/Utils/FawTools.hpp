#ifndef __FAW_TOOLS_HPP__
#define __FAW_TOOLS_HPP__

#include "../Core/UIManager.h"

namespace DuiLib {
	class FawTools {
	public:
		// "1,2,3,4" -> RECT
		template <typename T>
		static RECT parse_rect (T &str) { auto v = split_number (str, 4); return { (LONG) v[0], (LONG) v[1], (LONG) v[2], (LONG) v[3] }; }

		// "1.0,2.0,3.0,4.0" -> TPercentInfo
		template <typename T>
		static TPercentInfo parse_TPercentInfo (T &str) { auto v = split_double (str, 4); return { v[0], v[1], v[2], v[3] }; }

		// "1,2" -> SIZE
		template <typename T>
		static SIZE parse_size (T &str) { auto v = split_number (str, 2); return { (LONG) v[0], (LONG) v[1] }; }

		// "#FFFF0000" or "FF0000" -> size_t
		template <typename T>
		static size_t parse_hex (T &str) {
			size_t n = 0, i = (str.length () > 0 && str[0] == _T ('#')) ? 1 : 0;
			for (; i < str.length (); ++i) {
				TCHAR ch = str[i];
				if (ch >= _T ('0') && ch <= _T ('9')) { n = n * 16 + ch - _T ('0'); }
				else if (ch >= _T ('A') && ch <= _T ('Z')) { n = n * 16 + ch - _T ('A') + 10; }
				else if (ch >= _T ('a') && ch <= _T ('z')) { n = n * 16 + ch - _T ('a') + 10; }
				else { break; }
			}
			if constexpr (!std::is_const<T>::value)
				str = str.substr (i);
			return n;
		}

		// "12345" -> size_t
		template <typename T>
		static int parse_dec (T &str) {
			int n = 0;
			bool is_sign = (str.size () > 0 && str[0] == _T ('-'));
			size_t i = is_sign ? 1 : 0;
			for (; i < str.length (); ++i) {
				TCHAR ch = str[i];
				if (ch < _T ('0') || ch > _T ('9'))
					break;
				n = n * 10 + ch - _T ('0');
			}
			if constexpr (!std::is_const<T>::value)
				str = str.substr (i);
			return is_sign ? 0 - n : n;
		}

		// "true" or "True" or "TRUE" -> true
		static bool parse_bool (string_view_t str) {
			static std::map<string_view_t, bool> mbool { { _T ("true"), true }, { _T ("false"), false }, { _T ("on"), true }, { _T ("off"), false }, { _T ("yes"), true }, { _T ("no"), false }, { _T ("ok"), true }, { _T ("cancel"), false }, { _T ("1"), true }, { _T ("0"), false } };
			if (mbool.find (str) != mbool.end ())
				return mbool[str];
			return !str.empty ();
		}

		// "a"="a" "b"=b c="c" d=d -> std::map<string_t, string_t>
		static std::map<string_t, string_t> parse_keyvalue_pairs (string_view_t str) {
			std::map<string_t, string_t> m;
			size_t begin = 0;
			string_view_t str_key = _T (""), str_value = _T ("");
			while (begin < str.size ()) { 	// parse str_key
				TCHAR ch = str[begin];
				TCHAR sp = (ch == _T ('\"') || ch == _T ('\'') ? ch : _T ('='));
				if (ch == sp) ++begin;
				size_t p = str.find (sp, begin);
				if (p == string_t::npos) break;
				str_key = str.substr (begin, p - begin);
				if (p == string_t::npos) break;
				// parse str_value
				p = str.find (_T ('='), begin);
				if (p == string_t::npos) break;
				begin = p + 1;
				if (begin >= str.size ()) break;
				ch = str[begin];
				sp = (ch == _T ('\"') || ch == _T ('\'')) ? ch : _T (' ');
				if (ch == sp) {
					if (begin + 2 >= str.size ()) break;
					++begin;
				}
				p = str.find (sp, begin);
				if (p == string_t::npos) break;
				str_value = str.substr (begin, p - begin);
				// reset
				m[string_t (str_key)] = string_t (str_value);
				if (p != string_t::npos) {
					begin = p + 1;
					if (begin >= str.size ()) break;
					if (ch == sp && str[begin] == _T (' ')) ++begin;
				}
				str_key = str_value = _T ("");
			}
			if (!str_key.empty ())
				m[string_t (str_key)] = string_t (str_value);
			return m;
		}

		//
		static std::string format_strA (std::string_view str, ...) {
			if (str.empty ())
				return "";
			try {
				va_list ap;
				//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				ptrdiff_t final_n, n = (str.length ()) * 2;
				std::unique_ptr<char[]> formatted;
				while (true) {
					formatted.reset (new char[n]);
					//strcpy_s (&formatted [0], fmt_str.size (), fmt_str);
					va_start (ap, str);
					// _vsntprintf_s
					final_n = _vsnprintf_s (formatted.get (), n, _TRUNCATE, str.data (), ap);
					va_end (ap);
					if (final_n < 0 || final_n >= n)
						n += abs (final_n - n + 1);
					else
						break;
				}
				return formatted.get ();
			} catch (...) {
			}
			return "";
		}
		static std::wstring format_strW (std::wstring_view str, ...) {
			if (str.empty ())
				return L"";
			try {
				va_list ap;
				//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				ptrdiff_t final_n, n = (str.length ()) * 2;
				std::unique_ptr<wchar_t[]> formatted;
				while (true) {
					formatted.reset (new wchar_t[n]);
					//strcpy_s (&formatted [0], fmt_str.size (), fmt_str);
					va_start (ap, str);
					// _vsntprintf_s
					final_n = _vsnwprintf_s (formatted.get (), n, _TRUNCATE, str.data (), ap);
					va_end (ap);
					if (final_n < 0 || final_n >= n)
						n += abs (final_n - n + 1);
					else
						break;
				}
				return formatted.get ();
			} catch (...) {
			}
			return L"";
		}
		static string_t format_str (string_view_t str, ...) {
			if (str.empty ())
				return _T ("");
			try {
				va_list ap;
				//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				ptrdiff_t final_n, n = (str.length ()) * 2;
				std::unique_ptr<TCHAR[]> formatted;
				while (true) {
					formatted.reset (new TCHAR[n]);
					//strcpy_s (&formatted [0], fmt_str.size (), fmt_str);
					va_start (ap, str);
					final_n = _vsntprintf_s (formatted.get (), n, _TRUNCATE, str.data (), ap);
					va_end (ap);
					if (final_n < 0 || final_n >= n)
						n += abs (final_n - n + 1);
					else
						break;
				}
				return formatted.get ();
			} catch (...) {
			}
			return _T ("");
		}

		//
		template<typename T>
		static size_t find (std::vector<T> &v, T &item) {
			for (int i = 0; i < v.size (); ++i) {
				if (v[i] == item)
					return i;
			}
			return string_t::npos;
		}



		//
		// 字符串处理
		//

		static std::wstring gb18030_to_utf16 (std::string_view _old) { return _conv_to_wide (_old, CP_ACP); }
		static std::string utf16_to_gb18030 (std::wstring_view _old) { return _conv_to_multi (_old, CP_ACP); }
		static std::wstring utf8_to_utf16 (std::string_view _old) { return _conv_to_wide (_old, CP_UTF8); }
		static std::string utf16_to_utf8 (std::wstring_view _old) { return _conv_to_multi (_old, CP_UTF8); }
		static std::string gb18030_to_utf8 (std::string_view _old) { return utf16_to_utf8 (gb18030_to_utf16 (_old)); }
		static std::string utf8_to_gb18030 (std::string_view _old) { return utf16_to_gb18030 (utf8_to_utf16 (_old)); }
#ifdef UNICODE
		static std::string get_gb18030 (string_view_t _old) { return utf16_to_gb18030 (_old); }
		static std::string get_utf8 (string_view_t _old) { return utf16_to_utf8 (_old); }
		static std::wstring get_utf16 (string_view_t _old) { return std::wstring (_old); }
		static string_t get_T (std::string_view _old) { return gb18030_to_utf16 (_old); }
		static string_t get_T_from_utf8 (std::string_view _old) { return utf8_to_utf16 (_old); }
		static string_t get_T (std::wstring_view _old) { return std::wstring (_old); }
#else
		static std::string get_gb18030 (string_view_t _old) { return std::string (_old); }
		static std::string get_utf8 (string_view_t _old) { return gb18030_to_utf8 (_old); }
		static std::wstring get_utf16 (string_view_t _old) { return gb18030_to_utf16 (_old); }
		static string_t get_T (std::string_view _old) { return std::string (_old); }
		static string_t get_T_from_utf8 (std::string_view _old) { return utf8_to_gb18030 (_old); }
		static string_t get_T (std::wstring_view _old) { return utf16_to_gb18030 (_old); }
#endif

	private:
		// "1,2,3,4" -> std::vector<size_t>
		template <typename T>
		static std::vector<int64_t> split_number (T &str, size_t expect = string_t::npos) {
			std::vector<int64_t> v;
			int64_t n = 0;
			size_t i = 0;
			bool has_num = false, is_sign = false;
			for (; i < str.length (); ++i) {
				TCHAR ch = str[i];
				if (ch >= _T ('0') && ch <= _T ('9')) {
					n = n * 10 + ch - _T ('0');
					has_num = true;
				} else if (ch == _T ('-')) {
					is_sign = true;
				} else {
					v.push_back (is_sign ? -n : n);
					n = 0;
					has_num = false;
					is_sign = false;
					if (expect != string_t::npos && v.size () >= expect)
						break;
				}
			}
			if (has_num)
				v.push_back (is_sign ? -n : n);
			while (expect != string_t::npos && v.size () < expect)
				v.push_back (0);
			if constexpr (!std::is_const<T>::value)
				str = str.substr (i);
			return v;
		}

		template <typename T>
		static std::vector<double> split_double (T &str, size_t expect = string_t::npos) {
			std::vector<double> v;
			double n = 0.0, l = 1.0;
			size_t i = 0;
			bool has_num = false, is_sign = false;
			for (; i < str.length (); ++i) {
				TCHAR ch = str[i];
				if (ch >= _T ('0') && ch <= _T ('9')) {
					if (l > 0.5) {
						n = n * 10 + ch - _T ('0');
					} else {
						n += (ch - _T ('0')) * l;
						l /= 10;
					}
					has_num = true;
				} else if (ch == _T ('.')) {
					l /= 10;
				} else if (ch == _T ('-')) {
					is_sign = true;
				} else {
					v.push_back (is_sign ? -n : n);
					n = 0.0;
					has_num = false;
					if (expect != string_t::npos && v.size () >= expect)
						break;
				}
			}
			if (has_num)
				v.push_back (is_sign ? -n : n);
			while (expect != string_t::npos && v.size () < expect)
				v.push_back (0.0);
			if constexpr (!std::is_const<T>::value)
				str = str.substr (i);
			return v;
		}

		static std::string _conv_to_multi (std::wstring_view _old, UINT ToType) {
			int lenOld = (int) _old.length ();
			int lenNew = ::WideCharToMultiByte (ToType, 0, _old.data (), lenOld, nullptr, 0, nullptr, nullptr);
			std::string s ((size_t) (lenNew / 2 + 1) * 2, '\0');
			if (!::WideCharToMultiByte (ToType, 0, _old.data (), lenOld, &s[0], lenNew, nullptr, nullptr))
				return "";
			return s.c_str ();
		}

		static std::wstring _conv_to_wide (std::string_view _old, UINT ToType) {
			int lenOld = (int) _old.length ();
			int lenNew = ::MultiByteToWideChar (ToType, 0, _old.data (), lenOld, nullptr, 0);
			std::wstring s ((size_t) (lenNew / 2 + 1) * 2, L'\0');
			if (!::MultiByteToWideChar (ToType, 0, _old.data (), lenOld, &s[0], lenNew))
				return L"";
			return s.c_str ();
		}

		//static std::vector<string_t> split (string_view_t str, TCHAR sp) {
		//	std::vector<string_t> v;
		//	return v;
		//}
	};
}

#endif //__FAW_TOOLS_HPP__
