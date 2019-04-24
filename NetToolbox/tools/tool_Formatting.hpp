////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Formatting
// Description: 格式化工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_FORMATTING_HPP__
#define __TOOL_FORMATTING_HPP__

#include <string>
#include <regex>
#include <map>



class tool_Formatting {
public:
	// 查询名称所对应的正则表达式
	static std::string query_regex (std::string desp) {
		static std::map<std::string, std::string> m_regex_items {
			{ "IPv4", "^(((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))$" },
			{ "IPv6", "^((([0-9|A-F|a-f]{1,4}:){7}([0-9|A-F|a-f]{1,4}|:))|(([0-9|A-F|a-f]{1,4}:){6}(:[0-9|A-F|a-f]{1,4}|((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9|A-F|a-f]{1,4}:){5}(((:[0-9|A-F|a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9|A-F|a-f]{1,4}:){4}(((:[0-9|A-F|a-f]{1,4}){1,3})|((:[0-9|A-F|a-f]{1,4})?:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9|A-F|a-f]{1,4}:){3}(((:[0-9|A-F|a-f]{1,4}){1,4})|((:[0-9|A-F|a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9|A-F|a-f]{1,4}:){2}(((:[0-9|A-F|a-f]{1,4}){1,5})|((:[0-9|A-F|a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9|A-F|a-f]{1,4}:){1}(((:[0-9|A-F|a-f]{1,4}){1,6})|((:[0-9|A-F|a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(:(((:[0-9|A-F|a-f]{1,4}){1,7})|((:[0-9|A-F|a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:)))(%.+)?$" },
			{ International::translate ("Domain"), "^([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}$" },
			{ "URL", "^(http|ftp|https):\\/\\/[\\w\\-_]+(\\.[\\w\\-_]+)+([\\w\\-\\.,@?^=%&:/~\\+#]*[\\w\\-\\@?^=%&/~\\+#])?$" },
			{ International::translate ("E-mail"), "^\\w+(\\.\\w+)*@(\\w)+((\\.\\w+)+)$" },
			{ International::translate ("ID No."), "^(\\d{17}(\\d|x|X|))|(\\d{15})$" },
			{ International::translate ("Phone"), "^\\d{11}$" },
			{ International::translate ("Date"), "^\\d{4}-\\d{1,2}-\\d{1,2}$" },
			{ International::translate ("Time"), "^\\d{1,2}:\\d{1,2}:\\d{1,2}$" },
			{ International::translate ("Money"), "^([0-9]+|[0-9]{1,3}(,?[0-9]{3})*)(.[0-9]{1,5})?$" },
			{ International::translate ("Postcode"), "^\\d{6}$" },
			{ International::translate ("Chinese Character(GB18030)"), "^((\\u8140-\\ufefe)|(\\u8130-\\ufe39){2})+$" },
		};
		return m_regex_items[desp];
	}

	// 判断目标字符串是否为ipv4
	static bool is_ipv4 (std::string s) {
		std::regex r (query_regex ("IPv4"));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为ipv6
	static bool is_ipv6 (std::string s) {
		std::regex r (query_regex ("IPv6"));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为域名
	static bool is_domain (std::string s) {
		std::regex r (query_regex (International::translate ("Domain")));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为网址
	static bool is_url (std::string s) {
		std::regex r (query_regex ("URL"));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为手机
	static bool is_phone (std::string s) {
		std::regex r (query_regex (International::translate ("Phone")));
		return std::regex_match (s, r);
	}

	// 判断目标字符串是否为电子邮箱
	static bool is_email (std::string s) {
		std::regex r (query_regex (International::translate ("E-mail")));
		return std::regex_match (s, r);
	}
};

#endif //__TOOL_FORMATTING_HPP__
