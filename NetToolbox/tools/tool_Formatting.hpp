#ifndef __TOOL_FORMATTING_HPP__
#define __TOOL_FORMATTING_HPP__

#include <string>
#include <regex>
#include <map>

namespace faw {
#ifdef UNICODE
	typedef std::wregex regex_t;
#else
	typedef std::regex regex_t;
#endif
}



class tool_Formatting {
public:
	// 查询名称所对应的正则表达式
	static faw::string_t query_regex (faw::string_t desp) {
		static std::map<faw::string_t, faw::string_t> m_regex_items {
			{ _T ("IPv4"), _T ("^(((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))$") },
			{ _T ("IPv6"), _T ("^((([0-9|A-F|a-f]{1,4}:){7}([0-9|A-F|a-f]{1,4}|:))|(([0-9|A-F|a-f]{1,4}:){6}(:[0-9|A-F|a-f]{1,4}|((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9|A-F|a-f]{1,4}:){5}(((:[0-9|A-F|a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9|A-F|a-f]{1,4}:){4}(((:[0-9|A-F|a-f]{1,4}){1,3})|((:[0-9|A-F|a-f]{1,4})?:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9|A-F|a-f]{1,4}:){3}(((:[0-9|A-F|a-f]{1,4}){1,4})|((:[0-9|A-F|a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9|A-F|a-f]{1,4}:){2}(((:[0-9|A-F|a-f]{1,4}){1,5})|((:[0-9|A-F|a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9|A-F|a-f]{1,4}:){1}(((:[0-9|A-F|a-f]{1,4}){1,6})|((:[0-9|A-F|a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(:(((:[0-9|A-F|a-f]{1,4}){1,7})|((:[0-9|A-F|a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:)))(%.+)?$") },
			{ International::translate (_T ("Domain")), _T ("^([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}$") },
			{ _T ("URL"), _T ("^(http|ftp|https):\\/\\/[\\w\\-_]+(\\.[\\w\\-_]+)+([\\w\\-\\.,@?^=%&:/~\\+#]*[\\w\\-\\@?^=%&/~\\+#])?$") },
			{ International::translate (_T ("E-mail")), _T ("^\\w+(\\.\\w+)*@(\\w)+((\\.\\w+)+)$") },
			{ International::translate (_T ("ID No.")), _T ("^(\\d{17}(\\d|x|X|))|(\\d{15})$") },
			{ International::translate (_T ("Phone")), _T ("^\\d{11}$") },
			{ International::translate (_T ("Date")), _T ("^\\d{4}-\\d{1,2}-\\d{1,2}$") },
			{ International::translate (_T ("Time")), _T ("^\\d{1,2}:\\d{1,2}:\\d{1,2}$") },
			{ International::translate (_T ("Money")), _T ("^([0-9]+|[0-9]{1,3}(,?[0-9]{3})*)(.[0-9]{1,5})?$") },
			{ International::translate (_T ("Postcode")), _T ("^\\d{6}$") },
			{ International::translate (_T ("Chinese Character(GB18030)")), _T ("^((\\u8140-\\ufefe)|(\\u8130-\\ufe39){2})+$") },
			//{ International::translate (_T ("Chinese Character(UTF8)")), _T ("^#(\\S{4,8})($|\\s.+)$") },
		};
		return m_regex_items[desp];
	}

	// 判断目标字符串是否为ipv4
	static bool is_ipv4 (faw::string_t s) {
		faw::regex_t r (query_regex (_T ("IPv4")));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为ipv6
	static bool is_ipv6 (faw::string_t s) {
		faw::regex_t r (query_regex (_T ("IPv6")));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为域名
	static bool is_domain (faw::string_t s) {
		faw::regex_t r (query_regex (International::translate (_T ("Domain"))));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为网址
	static bool is_url (faw::string_t s) {
		faw::regex_t r (query_regex (_T ("URL")));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为手机
	static bool is_phone (faw::string_t s) {
		faw::regex_t r (query_regex (International::translate (_T ("Phone"))));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为电子邮箱
	static bool is_email (faw::string_t s) {
		faw::regex_t r (query_regex (International::translate (_T ("E-mail"))));
		return std::regex_match (s, r);
	}
};

#endif //__TOOL_FORMATTING_HPP__
