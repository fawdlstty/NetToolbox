#ifndef __TOOL_WEB_REQUEST_HPP__
#define __TOOL_WEB_REQUEST_HPP__

#include <string>
#include <string_view>
#include <tuple>
#include <initializer_list>

#include "tool_String.hpp"

#include "../3rdparty/Simple-Web-Server/client_https.hpp"



class tool_WebRequest {
public:
	static std::string get (std::string_view url) {
		try {
			auto[schema, host, path] = parse_url (url);
			if (schema == "http://") {
				SimpleWeb::Client<SimpleWeb::HTTP> client (host);
				auto req = client.request ("GET", path);
				return req->content.string ();
			} else if (schema == "https://") {
				SimpleWeb::Client<SimpleWeb::HTTPS> client (host, false);
				auto req = client.request ("GET", path);
				return req->content.string ();
			}
		} catch (...) {
		}
		return "";
	}

	static std::string post (std::string_view url, std::string_view data) {
		try {
			auto[schema, host, path] = parse_url (url);
			SimpleWeb::CaseInsensitiveMultimap header { { "Content-Type", "application/x-www-form-urlencoded" }, { "User-Agent", "NetToolbox" } };
			if (schema == "http://") {
				SimpleWeb::Client<SimpleWeb::HTTP> client (host);
				auto req = client.request ("POST", path, data, header);
				return req->content.string ();
			} else if (schema == "https://") {
				SimpleWeb::Client<SimpleWeb::HTTPS> client (host, false);
				auto req = client.request ("POST", path, data, header);
				return req->content.string ();
			}
		} catch (...) {
		}
		return "";
	}

protected:
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
