#ifndef __TOOL_JSON_HPP__
#define __TOOL_JSON_HPP__

// 此工具仅供jsoncpp使用
#include <nlohmann/json.hpp>
using json = nlohmann::json;
//#include "json11.hpp"
//using namespace json11;
//#include <json/json.h>
////#ifdef _DEBUG
////#pragma comment (lib, "jsoncpp-d.lib")
////#else
//#pragma comment (lib, "jsoncpp.lib")
////#endif

class tool_Json {
public:
	static std::string conv (json &_root) {
		//Json::StreamWriterBuilder _wb;
		//_wb ["commentStyle"] = "None";
		//_wb ["indentation"] = "";
		//return Json::writeString (_wb, _root);
		return _root.dump ();
	}

	static json conv (std::string _str) {
		//json _root;
		//Json::Reader ().parse (_str, _root, false);
		//return _root;
		return json::parse (_str);
	}
};

#endif //__TOOL_JSON_HPP__
