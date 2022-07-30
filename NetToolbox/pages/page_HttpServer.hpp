#ifndef __PAGE_HTTPSERVER_HPP__
#define __PAGE_HTTPSERVER_HPP__

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <thread>

#include "../../xfinal/xfinal/xfinal.hpp"

#include "../tools/tool_String.hpp"



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
	page_HttpServer (NetToolboxWnd *parent): page_base (parent) {
		_add_item (false);
		_add_item (true);
	}

	bool OnClick (TNotifyUI &msg) override {
		faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("httpserver_begin")) {
			if (!m_server) {
				m_indexes.clear ();
				tool_StringA::split (faw::Encoding::T_to_gb18030 (m_httpserver_index->GetText ()), m_indexes, '|');
				// check maplist
				m_maplist.clear ();
				for (int i = 0; i < m_httpserver_maplist->GetCount (); ++i) {
					CListContainerElementUI *_item = dynamic_cast<CListContainerElementUI *> (m_httpserver_maplist->GetItemAt (i));
					if (i < m_httpserver_maplist->GetCount () - 1) {
						faw::string_t _item_name = _item->GetName ();
						faw::string_t _key = BindEditUI { _item_name + _T ("_key") }->GetText ();
						faw::string_t _value = BindEditUI { _item_name + _T ("_value") }->GetText ();
						TCHAR _buf[MAX_PATH];
						_buf [0] = _T ('\0');
						faw::string_t _key2 = _key.data ();
						tool_StringT::replace (_key2, _T ('/'), _T ('\\'));
						DWORD _key_len = ::GetShortPathName (_key2.data (), _buf, sizeof (_buf));
						if (_key_len == 0) {
							m_parent->show_status (NetToolboxWnd::StatusIcon::Error, std::vformat (_IT (_T ("Invalid Path: {}")), std::make_format_args (_key)));
							return true;
						}
						if (_value.empty () || _value [0] != _T ('/')) {
							m_parent->show_status (NetToolboxWnd::StatusIcon::Error, std::vformat (_IT (_T ("Invalid Url Path: {}")), std::make_format_args (_value)));
							return true;
						}
						for (size_t i = 0; i < _value.size (); ++i) {
							static faw::string_t s_allow_signs = _T ("/-_+");
							char _ch = _value [i];
							if (!((_ch >= '0' && _ch <= '9') || (_ch >= 'a' && _ch <= 'z') || (_ch >= 'A' && _ch <= 'Z') || s_allow_signs.find (_ch) != faw::string_t::npos)) {
								m_parent->show_status (NetToolboxWnd::StatusIcon::Error, std::vformat (_IT (_T ("Invalid Url Path: {}")), std::make_format_args (_value)));
								return true;
							}
						}
						m_maplist.push_back ({ _buf, _value });
					}
				}

				// start server
				std::string _port_s = faw::Encoding::T_to_gb18030 (m_httpserver_port->GetText ());
				m_server = std::make_shared<xfinal::http_server> (1);
				if (!m_server->listen ("0.0.0.0", _port_s)) {
					m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _IT (_T ("Port listening failed. Please check whether the port is occupied.")));
					return true;
				}
				m_server->set_disable_auto_create_directories (true);
				m_server->set_session_storager (std::make_unique<_session_storage> ());
				m_server->set_not_found_callback (std::bind (&page_HttpServer::_callback, this, std::placeholders::_1, std::placeholders::_2));
				if (!m_thread) {
					m_thread = std::make_unique<std::thread> ([this] () { m_server->run (); });
				}
				m_httpserver_begin->SetText (_IT (_T ("Stop")));
				m_httpserver_port->SetEnabled (false);
				m_httpserver_index->SetEnabled (false);
				_enable_list (false);
			} else {
				m_server->stop ();
				if (m_thread) {
					if (m_thread->joinable ())
						m_thread->join ();
					m_thread = nullptr;
				}
				m_server = nullptr;
				m_httpserver_begin->SetText (_IT (_T ("Run")));
				m_httpserver_port->SetEnabled (true);
				m_httpserver_index->SetEnabled (true);
				_enable_list (true);
			}
			return true;
		} else if (name == _T ("httpserver_maplist_ctrl_up")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_httpserver_maplist->GetItemIndex (container);
			if (n == 0)
				return true;
			faw::string_t name1 { container->GetName () };
			faw::string_t name2 { m_httpserver_maplist->GetItemAt (n - 1)->GetName () };
			_swap_item (name1, name2);
			return true;
		} else if (name == _T ("httpserver_maplist_ctrl_down")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_httpserver_maplist->GetItemIndex (container);
			if (n >= m_httpserver_maplist->GetCount () - 2)
				return true;
			faw::string_t name1 { container->GetName () };
			faw::string_t name2 { m_httpserver_maplist->GetItemAt (n + 1)->GetName () };
			_swap_item (name1, name2);
			return true;
		} else if (name == _T ("httpserver_maplist_ctrl_new")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			m_httpserver_maplist->Remove (container);
			_add_item (false);
			_add_item (true);
			return true;
		} else if (name == _T ("httpserver_maplist_ctrl_del")) {
			CControlUI *container = msg.pSender->GetParent ()->GetParent ();
			int n = m_httpserver_maplist->GetItemIndex (container);
			m_httpserver_maplist->Remove (container);
			for (; n < m_httpserver_maplist->GetCount () - 1; ++n) {
				container = m_httpserver_maplist->GetItemAt (n);
				faw::string_t _name { container->GetName () };
				faw::string_t color = (n % 2) ? _T ("#FFDDDDDD") : _T ("#FFFFFFFF");
				faw::string_t _name1 = _name + _T ("_key");
				BindEditUI edit_key { _name1 };
				edit_key->SetAttribute (_T ("bkcolor"), color);
				edit_key->SetAttribute (_T ("nativebkcolor"), color);
				_name1 = _name + _T ("_value");
				BindEditUI edit_value { _name1 };
				edit_value->SetAttribute (_T ("bkcolor"), color);
				edit_value->SetAttribute (_T ("nativebkcolor"), color);
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
				_url = std::format (_T ("http://127.0.0.1:{}/"), _port);
				m_httpserver_begin->SetEnabled (true);
			}
			m_httpserver_addr->SetText (_url);
			return true;
		}
		return false;
	}

	void _callback (xfinal::request &req, xfinal::response &res) {
		std::string _url = std::string (req.url ());
		_url = faw::Encoding::utf8_to_gb18030 (_url);
		for (size_t i = 0; i < m_maplist.size (); ++i) {
			auto &[_disk_path, _url_path] = m_maplist [i];
			if (_url.compare (0, _url_path.size (), _url_path) == 0) {
				std::string _file_path = std::format (_T ("{}{}{}"), _disk_path, *_disk_path.crbegin () == '\\' ? "" : "\\", _url.substr (_url_path.size ()));
				tool_StringA::replace (_file_path, '/', '\\');
				tool_StringA::replace (_file_path, "\\\\", "\\");
				if (faw::File::exist (_file_path)) {
					res.write_file_view (_file_path, true);
					return;
				}
				if (_file_path [_file_path.size () - 1] != '\\')
					_file_path.push_back ('\\');
				for (size_t j = 0; j < m_indexes.size (); ++j) {
					std::string _file_path1 = _file_path + m_indexes [j];
					if (faw::File::exist (_file_path1)) {
						res.write_file_view (_file_path1, true);
						return;
					}
				}
			}
		}
		res.write_state (xfinal::http_status::not_found);
		res.write_string ("404 not found");
	}

protected:
	// add new line
	void _add_item (bool _is_new) {
		static size_t n_sign = 0;
		CListContainerElementUI *item = new CListContainerElementUI ();
		item->SetFixedHeight (20);
		faw::string_t color = (m_httpserver_maplist->GetCount () % 2) ? _T ("#FFEEEEEE") : _T ("#FFFFFFFF");
		if (_is_new)
			color = _T ("#FFDDDDDD");
		item->SetAttribute (_T ("name"), std::format (_T ("httpserver_maplist_item_{}"), ++n_sign));
		//
		CContainerUI *ctnr = nullptr;
		CControlUI *ctrl = nullptr;
		//
		ctnr = new CContainerUI ();
		ctrl = new CEditUI ();
		ctrl->SetManager (m_parent->get_pm (), item);
		ctrl->SetAttribute (_T ("name"), std::format (_T ("httpserver_maplist_item_{}_key"), n_sign));
		ctrl->SetAttribute (_T ("bkcolor"), color);
		ctrl->SetAttribute (_T ("nativebkcolor"), color);
		ctrl->SetAttribute (_T ("align"), _T ("center"));
		ctrl->SetAttribute (_T ("padding"), _T ("10,4,0,4"));
		if (_is_new) {
			ctrl->SetAttribute (_T ("enabled"), _T ("false"));
		} else {
			ctrl->SetAttribute (_T ("text"), _T ("D:\\wwwroot"));
		}
		ctnr->Add (ctrl);
		item->Add (ctnr);
		//
		ctnr = new CContainerUI ();
		ctrl = new CEditUI ();
		ctrl->SetManager (m_parent->get_pm (), item);
		ctrl->SetAttribute (_T ("name"), std::format (_T ("httpserver_maplist_item_{}_value"), n_sign));
		ctrl->SetAttribute (_T ("bkcolor"), color);
		ctrl->SetAttribute (_T ("nativebkcolor"), color);
		ctrl->SetAttribute (_T ("align"), _T ("center"));
		ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
		if (_is_new) {
			ctrl->SetAttribute (_T ("enabled"), _T ("false"));
		} else {
			ctrl->SetAttribute (_T ("text"), _T ("/"));
		}
		ctnr->Add (ctrl);
		item->Add (ctnr);
		//
		ctnr = new CHorizontalLayoutUI ();
		if (_is_new) {
			ctrl = new CButtonUI ();
			ctrl->SetManager (m_parent->get_pm (), ctnr);
			ctrl->SetText (_IT (_T ("Create")));
			ctrl->SetAttribute (_T ("style"), _T ("x_button"));
			ctrl->SetAttribute (_T ("name"), (_T ("httpserver_maplist_ctrl_new")));
			ctrl->SetAttribute (_T ("width"), (_T ("80")));
			ctnr->Add (ctrl);
		} else {
			ctrl = new CButtonUI ();
			ctrl->SetManager (m_parent->get_pm (), ctnr);
			ctrl->SetText (_T ("↑"));
			ctrl->SetAttribute (_T ("style"), _T ("x_button"));
			ctrl->SetAttribute (_T ("name"), _T ("httpserver_maplist_ctrl_up"));
			ctrl->SetAttribute (_T ("width"), _T ("20"));
			ctnr->Add (ctrl);
			ctrl = new CButtonUI ();
			ctrl->SetManager (m_parent->get_pm (), ctnr);
			ctrl->SetText (_T ("↓"));
			ctrl->SetAttribute (_T ("style"), _T ("x_button"));
			ctrl->SetAttribute (_T ("name"), _T ("httpserver_maplist_ctrl_down"));
			ctrl->SetAttribute (_T ("width"), _T ("20"));
			ctnr->Add (ctrl);
			ctrl = new CButtonUI ();
			ctrl->SetManager (m_parent->get_pm (), ctnr);
			ctrl->SetText (_IT (_T ("Delete")));
			ctrl->SetAttribute (_T ("style"), _T ("x_button"));
			ctrl->SetAttribute (_T ("name"), _T ("httpserver_maplist_ctrl_del"));
			ctrl->SetAttribute (_T ("width"), _T ("40"));
			ctnr->Add (ctrl);
		}
		item->Add (ctnr);
		m_httpserver_maplist->Add (item);
	}

	// swap two item content
	void _swap_item (faw::string_t name1, faw::string_t name2) {
		faw::string_t name_key1 = name1 + _T ("_key");
		BindEditUI edit_key1 { name_key1 };
		faw::string_t name_key2 = name2 + _T ("_key");
		BindEditUI edit_key2 { name_key2 };
		faw::string_t name_tmp = edit_key1->GetText ();
		edit_key1->SetText (edit_key2->GetText ());
		edit_key2->SetText (name_tmp);
		//
		faw::string_t name_value1 = name1 + _T ("_value");
		BindEditUI edit_value1 { name_value1 };
		faw::string_t name_value2 = name2 + _T ("_value");
		BindEditUI edit_value2 { name_value2 };
		name_tmp = edit_value1->GetText ();
		edit_value1->SetText (edit_value2->GetText ());
		edit_value2->SetText (name_tmp);
	}

	// lock when running
	void _enable_list (bool _enable) {
		for (int i = 0; i < m_httpserver_maplist->GetCount (); ++i) {
			CListContainerElementUI *_item = dynamic_cast<CListContainerElementUI *> (m_httpserver_maplist->GetItemAt (i));
			if (i < m_httpserver_maplist->GetCount () - 1) {
				std::string _item_name = _item->GetName ();
				BindEditUI { _item_name + _T ("_key") }->SetEnabled (_enable);
				BindEditUI { _item_name + _T ("_value") }->SetEnabled (_enable);
			}
			CHorizontalLayoutUI *_itemcol = dynamic_cast<CHorizontalLayoutUI*> (_item->GetItemAt (2));
			for (int j = 0; j < _itemcol->GetCount (); ++j) {
				_itemcol->GetItemAt (j)->SetEnabled (_enable);
			}
		}
	}

	BindButtonUI										m_httpserver_begin { _T ("httpserver_begin") };
	BindEditUI											m_httpserver_index { _T ("httpserver_index") };
	BindEditUI											m_httpserver_port { _T ("httpserver_port") };
	BindEditUI											m_httpserver_addr { _T ("httpserver_addr") };
	BindListUI											m_httpserver_maplist { _T ("httpserver_maplist") };

	std::shared_ptr<xfinal::http_server>				m_server;
	std::unique_ptr<std::thread>						m_thread;

	std::vector<std::tuple<std::string, std::string>>	m_maplist;
	std::vector<std::string>							m_indexes;
};

#endif //__PAGE_HTTPSERVER_HPP__
