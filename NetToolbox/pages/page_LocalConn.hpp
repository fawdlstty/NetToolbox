#ifndef __PAGE_LOCAL_CONNECTION_HPP__
#define __PAGE_LOCAL_CONNECTION_HPP__

#include <algorithm>

#include "page_base.hpp"
#include "../tools/tool_NetInfo.hpp"
#include "../tools/tool_Process.hpp"
#include "../tools/tool_Utils.hpp"
#include "../tools/tool_Priv.hpp"


class page_LocalConn: public page_base {
	using conn_item_t = std::tuple<bool, faw::string_t, uint16_t, faw::string_t, uint16_t, faw::string_t, DWORD, faw::string_t>;
public:
	page_LocalConn (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_LocalConn () {
		CMenuWnd::DestroyMenu ();
		if (m_menu) {
			delete m_menu;
			m_menu = nullptr;
		}
	}

	bool OnClick (TNotifyUI& msg) override {
		 faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("localnet_tcplisten") || name == _T ("localnet_tcpconn") || name == _T ("localnet_udp")) {
			m_show_type = msg.pSender->GetText ();
			_update_data (false);
			return true;
		}
		return false;
	}

	bool OnRButtonDown (POINT &pt) override {
		CControlUI *ctrl = m_parent->find_control (pt);
		while (ctrl->GetClass () != _T ("ListContainerElementUI")) {
			if ((ctrl = ctrl->GetParent ()) == nullptr)
				break;
		}
		if (ctrl) {
			//CMenuUI   DUI_MSGTYPE_MENU
			m_sel_pid = _ttoi (ctrl->GetCustomAttribute (_T ("pid")).data ());
			m_sel_pname = ctrl->GetCustomAttribute (_T ("pname"));
			CMenuWnd::DestroyMenu ();
			::ClientToScreen (m_parent->GetHWND (), &pt);
			m_parent->async_invoke ([this, pt] () -> LRESULT {
				if (m_menu) {
					delete m_menu;
					m_menu = nullptr;
				}
				CMenuWnd::GetGlobalContextMenuObserver ().SetMenuCheckInfo (&m_MenuInfos);
				m_menu = new CMenuWnd ();
				m_menu->Init (nullptr, _IT (_T ("menu_localnet.xml")), pt, m_parent->get_pm ());
				return 0;
			});
			return true;
		}
		return false;
	}

	void ui_update_data () override { _update_data (true); }

	bool OnHeaderClick (TNotifyUI &msg) {
		if (msg.pSender->GetParent ()->GetParent ()->GetName () == _T ("localnet_connection")) {
			 faw::string_t name = msg.pSender->GetText ();
			if (name == m_sort_str) {
				m_is_increment = !m_is_increment;
			} else {
				m_sort_str = name;
				m_is_increment = true;
			}
			_update_data (false);
			return true;
		}
		return false;
	}

	bool OnMenuClick (MenuCmd *mc) override {
		if (mc->szName == _T ("menu_localnet_kill")) {
			faw::string_t tip_info = fmt::format (_IT (_T ("Are you sure you want to terminate the process {} (pid:{})?")), m_sel_pname, m_sel_pid);
			if (IDOK == ::MessageBox (m_parent->GetHWND (), tip_info.data (), _IT (_T ("Info")).data (), MB_ICONQUESTION | MB_OKCANCEL)) {
				if (tool_Process::kill ((DWORD) m_sel_pid)) {
					ui_update_data ();
				} else {
					// 进程无法结束
					if (tool_Priv::is_admin ()) {
						// Admin权限：提示失败
						::MessageBox (m_parent->GetHWND (), _IT ( _T ("Process could not terminate!")).data (), _IT (_T ("Info")).data (), MB_ICONWARNING);
					} else if (tool_Priv::adjust_restart (m_parent->m_sel1, m_parent->m_sel2)) {
						// 非Admin权限：提权并退出
						m_parent->Close ();
					}
				}
			}
			return true;
		} else if (mc->szName == _T ("menu_localnet_show")) {
			auto[err, path] = tool_Process::get_path ((DWORD) m_sel_pid);
			if (path.empty ()) {
				// 路径无法显示
				if (tool_Priv::is_admin ()) {
					// Admin权限：提示失败
					faw::string_t err_str = tool_Utils::get_error_info (err);
					::MessageBox (m_parent->GetHWND (), err_str.data (), _IT (_T ("Info")).data (), MB_ICONWARNING);
				} else if (tool_Priv::adjust_restart (m_parent->m_sel1, m_parent->m_sel2)) {
					// 非Admin权限：提权并退出
					m_parent->Close ();
				}
				return true;
			}
			faw::Directory::show_path (path);
			return true;
		}
		return false;
	}

protected:
	void _update_data (bool bUpdateData) {
		static size_t i, x = 0;
		if (bUpdateData && x++ % 5 > 0)
			return;
		bool bIsEqual = true;
		static std::vector<conn_item_t> vconns, type_vconns;
		std::vector<conn_item_t> _vconns, _type_vconns;
		if (bUpdateData) {
			_vconns = tool_NetInfo::get_connections ();
			bIsEqual = (vconns.size () == _vconns.size ());
			if (bIsEqual) {
				for (i = 0; i < vconns.size (); ++i) {
					if (vconns[i] != _vconns[i]) {
						bIsEqual = false;
						break;
					}
				}
			}
		} else {
			type_vconns.clear ();
		}

		if (!bUpdateData || !bIsEqual) {
			if (bUpdateData) {
				vconns.swap (_vconns);
				std::sort (vconns.begin (), vconns.end (), [this] (const conn_item_t &_v1, const conn_item_t &_v2) -> bool {
					if (m_sort_str == _IT (_T ("Local Addr"))) {
						faw::string_t _val1 = std::get<1> (_v1);
						faw::string_t _val2 = std::get<1> (_v2);
						if (_val1 == _val2)
							return false;
						return (_val1 < _val2) == m_is_increment;
					} else if (m_sort_str == _IT (_T ("Local Port"))) {
						uint16_t _val1 = std::get<2> (_v1);
						uint16_t _val2 = std::get<2> (_v2);
						if (_val1 == _val2)
							return false;
						return (_val1 < _val2) == m_is_increment;
					} else if (m_sort_str == _IT (_T ("Remote Addr"))) {
						faw::string_t _val1 = std::get<3> (_v1);
						faw::string_t _val2 = std::get<3> (_v2);
						if (_val1 == _val2)
							return false;
						return (_val1 < _val2) == m_is_increment;
					} else if (m_sort_str == _IT (_T ("Remote Port"))) {
						uint16_t _val1 = std::get<4> (_v1);
						uint16_t _val2 = std::get<4> (_v2);
						if (_val1 == _val2)
							return false;
						return (_val1 < _val2) == m_is_increment;
					} else if (m_sort_str == _IT (_T ("Process ID"))) {
						DWORD _val1 = std::get<6> (_v1);
						DWORD _val2 = std::get<6> (_v2);
						if (_val1 == _val2)
							return false;
						return (_val1 < _val2) == m_is_increment;
					} else if (m_sort_str == _IT (_T ("Process Name"))) {
						faw::string_t _val1 = std::get<7> (_v1);
						faw::string_t _val2 = std::get<7> (_v2);
						if (_val1 == _val2)
							return false;
						return (_val1 < _val2) == m_is_increment;
					}
					return _v1 < _v2;
				});
			}
			for (i = 0; i < vconns.size (); ++i) {
				auto[is_ipv4/**/, local_addr, local_port, remote_addr, remote_port, conn_state/**/, pid, pname] = vconns[i];
				if (conn_state == m_show_type)
					_type_vconns.push_back (vconns[i]);
			}
			bIsEqual = (type_vconns.size () == _type_vconns.size ());
			if (bIsEqual) {
				for (i = 0; i < _type_vconns.size (); ++i) {
					if (_type_vconns[i] != type_vconns[i]) {
						bIsEqual = false;
						break;
					}
				}
			}
			if (bUpdateData && bIsEqual)
				return;
			type_vconns.swap (_type_vconns);

			m_localnet_connection->RemoveAll ();
			for (i = 0; i < type_vconns.size (); ++i) {
				//是否是ipv4（不显示），本地地址，本地端口，远程地址，远程端口，当前连接状态，进程PID，进程名称，进程路径（不显示）
				auto[is_ipv4/**/, local_addr, local_port, remote_addr, remote_port, conn_state/**/, pid, pname] = type_vconns[i];
				CListContainerElementUI *item = new CListContainerElementUI ();
				item->AddCustomAttribute (_T ("pid"), fmt::format (_T ("{}"), pid));
				item->AddCustomAttribute (_T ("pname"), pname);
				item->SetFixedHeight (20);
				//
				CControlUI *ctrl = new CControlUI ();
				ctrl->SetBkImage (_T ("file='list_ico_1.png' source='0,0,16,16' dest='0,0,16,16'"));
				//ctrl->SetText (is_ipv4 ? _T ("TCP") : _T ("UDP"));
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("7,2,7,2"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (local_addr.data ());
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("10,4,0,4"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (fmt::format (_T ("{}"), local_port));
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (remote_addr.data ());
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (remote_addr == _T ("*") ? _T ("*") : fmt::format (_T ("{}"), remote_port));
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				//
				//ctrl = new CTextUI ();
				//ctrl->SetText (conn_state.data ());
				//ctrl->SetAttribute (_T ("align"), _T ("center"));
				//ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				//item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (fmt::format (_T ("{}"), pid));
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (pname.data ());
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				m_localnet_connection->Add (item);
			}
		}
	}

protected:
	BindListUI			m_localnet_connection { _T ("localnet_connection") };
	faw::string_t			m_sort_str = _IT (_T ("Local Port"));
	faw::string_t			m_show_type = _IT (_T ("TCP Listening"));
	bool				m_is_increment = true;

	CStdStringPtrMap	m_MenuInfos;
	CMenuWnd			*m_menu = nullptr;
	size_t				m_sel_pid = 0;
	faw::string_t			m_sel_pname = _T ("");
};

#endif //__PAGE_LOCAL_CONNECTION_HPP__
