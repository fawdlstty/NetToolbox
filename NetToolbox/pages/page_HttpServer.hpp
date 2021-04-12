#ifndef __PAGE_HTTPSERVER_HPP__
#define __PAGE_HTTPSERVER_HPP__

#include <cstdint>
#include <memory>
#include <string>
#include <thread>

#include <fmt/core.h>
#include "../../xfinal/xfinal/xfinal.hpp"



struct cors {
	bool before (xfinal::request &req, xfinal::response &res) {
		res.add_header ("Cache-control", "no-store");
		res.add_header ("Access-Control-Allow-Origin", "*");
		res.add_header ("Access-Control-Allow-Headers", "X-Requested-With,Content-Type,Accept");
		res.add_header ("Access-Control-Allow-Methods", "GET,POST,PUT,OPTIONS");
		if (req.method () == "OPTIONS") {
			res.write_string ("GET,POST,PUT,OPTIONS");
			return false;
		}
		return true;
	}

	bool after (xfinal::request &req, xfinal::response &res) {
		return true;
	}
};

class _session_storage: public xfinal::session_storage {
public:
	bool init () { return true; }				// 初始化操作，从介质读取数据
	void save (xfinal::session &session) {}		// 用来保存session
	void remove (xfinal::session &session) {}	// 用来移除session
	void remove (std::string const &uuid) {}	// 通过uuid 删除session
};

class page_HttpServer: public page_base {
public:
	page_HttpServer (NetToolboxWnd *parent): page_base (parent) {}

	bool OnClick (TNotifyUI &msg) override {
		faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("httpserver_begin")) {
			if (!m_thread) {
				std::string _port_s = faw::Encoding::T_to_gb18030 (m_httpserver_port->GetText ());
				if (!m_server.listen ("0.0.0.0", _port_s)) {
					m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _IT (_T ("Port listening failed. Please check whether the port is occupied.")));
					return true;
				}
				m_server.set_disable_auto_create_directories (true);
				m_server.set_session_storager (std::make_unique<_session_storage> ());
				m_server.set_not_found_callback (std::bind (&page_HttpServer::_callback, this, std::placeholders::_1, std::placeholders::_2));
				m_thread = std::make_unique<std::thread> ([this] () {
					m_server.run ();
				});
				m_httpserver_begin->SetText (_IT (_T ("Stop")));
				m_httpserver_port->SetEnabled (false);
				m_httpserver_index->SetEnabled (false);
			} else {
				m_server.stop ();
				if (m_thread) {
					if (m_thread->joinable ())
						m_thread->join ();
					m_thread = nullptr;
				}
				m_httpserver_begin->SetText (_IT (_T ("Run")));
				m_httpserver_port->SetEnabled (true);
				m_httpserver_index->SetEnabled (true);
			}
			return true;
		}
		return false;
	}

	bool OnTextChanged (TNotifyUI &msg) override {
		faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("httpserver_port")) {
			faw::string_t _port_s = m_httpserver_port->GetText ();
			int _port = std::stoi (_port_s);
			faw::string_t _port_s2 = faw::Encoding::gb18030_to_T (std::to_string (_port));
			faw::string_t _url = _T ("");
			if (_port_s != _port_s2 || _port < 1 || _port > 65535) {
				_url = _IT (_T ("Invalid Port"));
				m_httpserver_begin->SetEnabled (false);
			} else if (_port == 80) {
				_url = _T ("http://127.0.0.1/");
				m_httpserver_begin->SetEnabled (true);
			} else {
				_url = fmt::format (_T ("http://127.0.0.1:{}/"), _port);
				m_httpserver_begin->SetEnabled (true);
			}
			m_httpserver_addr->SetText (_url);
			return true;
		}
		return false;
	}

	void _callback (xfinal::request &req, xfinal::response &res) {
		// TODO: 实现文件转发；拷贝xml实现
		//for (int i = 0; i < m_httpserver_maplist->GetCount (); ++i) {
		//	auto _item = m_httpserver_maplist->GetItemAt (i);
		//}
		res.write_string ("404 not found");
	}

protected:
	BindButtonUI					m_httpserver_begin { _T ("httpserver_begin") };
	BindEditUI						m_httpserver_index { _T ("httpserver_index") };
	BindEditUI						m_httpserver_port { _T ("httpserver_port") };
	BindEditUI						m_httpserver_addr { _T ("httpserver_addr") };
	BindListUI						m_httpserver_maplist { _T ("httpserver_maplist") };

	xfinal::http_server				m_server { 1 };
	std::unique_ptr<std::thread>	m_thread;
};

#endif //__PAGE_HTTPSERVER_HPP__
