////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_WebRequest
// Description: 网络请求工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_WEB_REQUEST_HPP__
#define __TOOL_WEB_REQUEST_HPP__

#include <string>
#include <tuple>
#include <initializer_list>

#include "tool_String.hpp"

//#define _TOOL_WEBREQUEST_USE_CURL
#ifdef _TOOL_WEBREQUEST_USE_CURL
#	include <curl/curl.h>
#	ifdef _DEBUG
#		pragma comment (lib, "libcurl-d_imp.lib")
#	else
#		pragma comment (lib, "libcurl_imp.lib")
#	endif
#else
#	include "../3rdparty/Simple-Web-Server/client_https.hpp"
#endif



class tool_WebRequest {
public:
	// 发起get请求
	static std::string get (std::string_view _url) {
		std::string _ret = "";
#ifdef _TOOL_WEBREQUEST_USE_CURL
		CURL *_curl = curl_easy_init ();
		curl_easy_setopt (_curl, CURLOPT_URL, _url.c_str ());
		curl_easy_setopt (_curl, CURLOPT_WRITEFUNCTION, _write_data);
		curl_easy_setopt (_curl, CURLOPT_WRITEDATA, &_ret);
		/*CURLcode code =*/ curl_easy_perform (_curl);
		curl_easy_cleanup (_curl);
#else
		try {
			std::string _schema = "", _host = "", _path = "";
			std::tie (_schema, _host, _path) = parse_url (_url);
			if (_schema == "http://") {
				SimpleWeb::Client<SimpleWeb::HTTP> client (_host);
				auto req = client.request ("GET", _path);
				_ret = req->content.string ();
			} else if (_schema == "https://") {
				SimpleWeb::Client<SimpleWeb::HTTPS> client (_host, false);
				auto req = client.request ("GET", _path);
				_ret = req->content.string ();
			}
		} catch (...) {
		}
#endif
		return _ret;
	}

	// 发起post请求
	static std::string post (std::string _url, std::string _data) {
		std::string _ret = "";
#ifdef _TOOL_WEBREQUEST_USE_CURL
		CURL *_curl = curl_easy_init ();
		curl_easy_setopt (_curl, CURLOPT_URL, _url.c_str ());
		curl_easy_setopt (_curl, CURLOPT_WRITEFUNCTION, _write_data);
		curl_easy_setopt (_curl, CURLOPT_WRITEDATA, &_ret);
		curl_slist *_headers = curl_slist_append (nullptr, "Content-Type: application/json");//x-www-form-urlencoded
		_headers = curl_slist_append (_headers, "User-Agent: NetToolbox");
		curl_easy_setopt (_curl, CURLOPT_HTTPHEADER, _headers);
		curl_easy_setopt (_curl, CURLOPT_POSTFIELDS, _data.c_str ());
		curl_easy_setopt (_curl, CURLOPT_POSTFIELDSIZE, _data.size ());
		/*CURLcode code =*/ curl_easy_perform (_curl);
		curl_slist_free_all (_headers);
		curl_easy_cleanup (_curl);
#else
		try {
			std::string _schema = "", _host = "", _path = "";
			std::tie (_schema, _host, _path) = parse_url (_url);
			SimpleWeb::CaseInsensitiveMultimap header { { "Content-Type", "application/x-www-form-urlencoded" }, { "User-Agent", "NetToolbox" } };
			if (_schema == "http://") {
				SimpleWeb::Client<SimpleWeb::HTTP> client (_host);
				auto req = client.request ("POST", _path, _data, header);
				_ret = req->content.string ();
			} else if (_schema == "https://") {
				SimpleWeb::Client<SimpleWeb::HTTPS> client (_host, false);
				auto req = client.request ("POST", _path, _data, header);
				_ret = req->content.string ();
			}
		} catch (...) {
		}
#endif
		return _ret;
	}

protected:
	// 解析url
	static std::tuple<std::string, std::string, std::string> parse_url (std::string_view url) {
		size_t p = url.find ("//");
		std::string schema = "http://";
		if (p != std::string::npos) {
			p += 2;
			schema = url.substr (0, p);
			url = url.substr (p);
		}
		p = url.find ("/");
		std::string host (url.substr (0, p));
		std::string path (p >= url.size () ? "/" : url.substr (p));
		return { schema, host, path };
	}
};

#endif //__TOOL_WEB_REQUEST_HPP__
