#include "StdAfx.h"
#include "NetToolboxWnd.h"

#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <memory>
#include <ws2ipdef.h>
#include <iphlpapi.h>

#include "tools/tool_String.hpp"
#include "tools/tool_Utils.hpp"
#include "tools/tool_Process.hpp"

#include "pages/page_base.hpp"
//
#include "pages/page_SysInfo.hpp"
#include "pages/page_LocalNet.hpp"
#include "pages/page_LocalConn.hpp"
//
//
//
#include "pages/page_Tracert.hpp"
#include "pages/page_Http.hpp"
#include "pages/page_Qps.hpp"
#include "pages/page_HttpServer.hpp"
#include "pages/page_IPScan.hpp"
//
#include "pages/page_Regex.hpp"
#include "pages/page_Rsa.hpp"
#include "pages/page_EncDec.hpp"
//
#include "pages/page_Gif.hpp"
#include "pages/page_SerialPort.hpp"
#include "pages/page_Window.hpp"
#include "pages/page_File.hpp"
#include "pages/page_QRcode.hpp"
#include "pages/page_Record.hpp"
//



void NetToolboxWnd::InitWindow () {
	//窗口初始化事件
	::SendMessage (m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM) m_icon);

	// 设置标题
	m_text_caption->SetText (m_caption.data ());

	m_pages = {
		{ new page_SysInfo (this), new page_LocalNet (this), new page_LocalConn (this) },
		{ nullptr },
		{ nullptr },
		{ new page_Tracert (this), new page_Http (this), new page_Qps (this), new page_HttpServer (this), new page_IPScan (this) },
		{ new page_Regex (this), new page_Rsa (this), new page_EncDec (this) },
		{ new page_Gif (this), new page_SerialPort (this), new page_Window (this), new page_File (this), new page_QRcode (this), new page_Record (this) },
		{ nullptr }
	};

	std::vector<faw::string_t> _args;
	auto cmd_line = ::GetCommandLineW ();
	int _argc = 0;
	LPWSTR *_argv = ::CommandLineToArgvW (cmd_line, &_argc);
	for (int i = 0; i < _argc; ++i)
		_args.push_back (faw::Encoding::utf16_to_T (_argv [i]));
	std::transform (_args [0].begin (), _args [0].end (), _args [0].begin (), [] (TCHAR ch) { return ch == _T ('/') ? _T ('\\') : ch; });
//#ifdef _DEBUG
//	if (_args.size () < 2) {
//		_args.push_back (_T ("-jump"));
//		_args.push_back (_T ("0,0"));
//	}
//#endif
	for (size_t i = 1; i < _args.size (); ++i) {
		if (_args[i] == _T ("-jump") && i < _args.size () - 1) {
			size_t p = _args[i + 1].find (_T (','));
			size_t sel1 = _ttoi (&_args[i + 1][0]), sel2 = _ttoi (&_args[i + 1][p + 1]);
			direct_page (sel1, sel2);
		}
	}
	::SetTimer (m_hWnd, (UINT_PTR) this, 1000, nullptr);

	//if (!m_notifyicon)
	//	m_notifyicon = new hNotifyIcon (m_hWnd, m_icon, NetToolboxWnd::lp_window_name, WM_NOTIFY_ICON);

	//exec_settings ();
}

void NetToolboxWnd::OnClick (TNotifyUI& msg) {
	 faw::string_t name = msg.pSender->GetName ();
	if (name == _T ("btn_set")) {
		//static int t = 0;
		//show_error_code (++t);
	//} else if (name == _T ("step_recorder")) {
	//	tool_Process::shell_exec (_T ("psr.exe"));
	//} else if (name == _T ("btn_blog")) {
	//	tool_Process::shell_exec (_T ("https://www.fawdlstty.com"));
	//} else if (name == _T ("btn_join")) {
	//	tool_Process::shell_exec (_T ("https://jq.qq.com/?_wv=1027&k=5TMvF3B"));
	} else if (name.substr (0, 3) == _T ("tab") || name.substr (0, 5) == _T ("group")) {
		WindowImplBase::OnClick (msg);
	} else if (name == _T ("about_home")) {
		tool_Process::shell_exec (_T ("https://nettoolbox.fawdlstty.com"));
	} else if (name == _T ("about_source")) {
		tool_Process::shell_exec (_T ("https://github.com/fawdlstty/NetToolbox"));
	} else {
		page_base *ppage = get_moment_page ();
		if (!ppage || !ppage->OnClick (msg))
			WindowImplBase::OnClick (msg);
	}
}

void NetToolboxWnd::OnHeaderClick (TNotifyUI& msg) {
	page_base *ppage = get_moment_page ();
	if (!ppage || !ppage->OnHeaderClick (msg))
		WindowImplBase::OnHeaderClick (msg);
}

void NetToolboxWnd::OnSelectChanged (TNotifyUI& msg) {
	 faw::string_t name = msg.pSender->GetName ();
	//标签页切换事件
	if (name.substr (0, 3) == _T ("tab")) {
		//TCHAR s[16] = _T ("tab?");
		//s[3] = name[3];
		//int idx = (int) (name[5] - _T ('0'));
		//BindTabLayoutUI ctrl { s };
		//ctrl->SelectItem (idx);
		//m_sel1 = m_tabM->GetCurSel ();
		//m_sel2 = m_tab[m_sel1]->GetCurSel ();
		//ui_update_data ();
		m_tabM->SelectItem ((int) (m_sel1 = _ttoi (&name[3])));
		m_tab[m_sel1]->SelectItem ((int) (m_sel2 = _ttoi (&name[5])));
		ui_update_data ();
	} else if (name.substr (0, 5) == _T ("group")) {
		size_t sel1 = _ttoi (&name[5]);
		for (size_t i = 0; i < m_tab.size (); ++i) {
			faw::string_t name1 = std::format (_T ("group{}"), i);
			BindOptionUI option { name1 };
			faw::string_t name2 = std::format (_T ("group_item{}"), i);
			BindVerticalLayoutUI vertical { name2 };
			if (vertical) {
				option->SetTextColor (i == sel1 ? 0xFFFFFFB0 : 0xFFFFFFFF);
				vertical->SetVisible (i == sel1);
			}
		}
	} else {
		page_base *ppage = get_moment_page ();
		if (ppage && ppage->OnSelectChanged (msg))
			return;
		WindowImplBase::OnSelectChanged (msg);
	}
}

void NetToolboxWnd::OnTextChanged (TNotifyUI& msg) {
	page_base *ppage = get_moment_page ();
	if (ppage && ppage->OnTextChanged (msg))
		return;
	WindowImplBase::OnTextChanged (msg);
}

void NetToolboxWnd::OnItemSelect (TNotifyUI& msg) {
	page_base *ppage = get_moment_page ();
	if (ppage && ppage->OnItemSelect (msg))
		return;
	WindowImplBase::OnItemSelect (msg);
}

void NetToolboxWnd::OnTimer (TNotifyUI& msg) {
	if (!enum_pages ([&msg] (page_base *page) { return page->OnTimer (msg); })) {
		WindowImplBase::OnTimer (msg);
	}
}

void NetToolboxWnd::Notify (TNotifyUI& msg) {
	if (msg.sType == _T ("menu")) {

	}
	WindowImplBase::Notify (msg);
}

void NetToolboxWnd::OnDropFiles (HDROP hDrop) {
	TCHAR tBuf[MAX_PATH] = { 0 };
	::DragQueryFile (hDrop, 0, tBuf, MAX_PATH);
	::DragFinish (hDrop);
	page_base *ppage = get_moment_page ();
	if (!ppage || !ppage->OnDropFiles (tBuf)) {
		//WindowImplBase::OnDropFiles (msg);
	}
}

std::optional<LRESULT> NetToolboxWnd::OnLButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam) {
	POINT pt = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
	page_base *ppage = get_moment_page ();
	if (ppage ? !!ppage->OnLButtonDown (pt) : FALSE) {
		return 0;
	} else {
		return WindowImplBase::OnLButtonDown (uMsg, wParam, lParam);
	}
}

std::optional<LRESULT> NetToolboxWnd::OnLButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam) {
	POINT pt = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
	page_base *ppage = get_moment_page ();
	if (ppage ? !!ppage->OnLButtonUp (pt) : FALSE) {
		return 0;
	} else {
		return WindowImplBase::OnLButtonUp (uMsg, wParam, lParam);
	}
}

std::optional<LRESULT> NetToolboxWnd::OnRButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam) {
	POINT pt = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
	page_base *ppage = get_moment_page ();
	if (ppage ? !!ppage->OnRButtonDown (pt) : FALSE) {
		return 0;
	} else {
		return WindowImplBase::OnRButtonDown (uMsg, wParam, lParam);
	}
}

std::optional<LRESULT> NetToolboxWnd::OnRButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam) {
	POINT pt = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
	page_base *ppage = get_moment_page ();
	if (ppage ? !!ppage->OnRButtonUp (pt) : FALSE) {
		return 0;
	} else {
		return WindowImplBase::OnRButtonUp (uMsg, wParam, lParam);
	}
}

std::optional<LRESULT> NetToolboxWnd::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam) {
	POINT pt = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
	page_base *ppage = get_moment_page ();
	if (ppage ? !!ppage->OnMouseMove (pt) : FALSE) {
		return 0;
	} else {
		return WindowImplBase::OnMouseMove (uMsg, wParam, lParam);
	}
}

LRESULT NetToolboxWnd::HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (WM_USER + 0x101 == uMsg) {
		auto f = reinterpret_cast<std::function<LRESULT ()>*>(lParam);
		LRESULT r = (*f) ();
		if (!wParam)
			delete f;
		return r;
	} else if (WM_TIMER == uMsg) {
		if (wParam == (WPARAM) this) {
			ui_update_data ();
			return 0;
		}
	} else if (WM_DROPFILES == uMsg) {
		OnDropFiles ((HDROP) wParam);
		return 0;
	} else if (WM_MENUCLICK == uMsg) {
		MenuCmd *mc = reinterpret_cast<MenuCmd*> (wParam);
		if (mc) {
			page_base *ppage = get_moment_page ();
			if (ppage->OnMenuClick (mc))
				return 0;
		}
	}
	return WindowImplBase::HandleMessage (uMsg, wParam, lParam);
}

CControlUI* NetToolboxWnd::CreateControl (faw::string_t pstrClass) {
	if (pstrClass == _T ("HanAnim")) return CHanAnimUI::CreateControl ();
	return WindowImplBase::CreateControl (pstrClass);
}



void NetToolboxWnd::show_status (StatusIcon _icon, faw::string_t _info) {
	static std::map<StatusIcon, faw::string_t> _status_images {
		{ StatusIcon::Ok, _T ("file='status_small.png' source='0,0,16,16'") },
		{ StatusIcon::Info, _T ("file='status_small.png' source='16,0,32,16'") },
		{ StatusIcon::Warning, _T ("file='status_small.png' source='0,16,16,32'") },
		{ StatusIcon::Error, _T ("file='status_small.png' source='16,16,32,32'") },
		{ StatusIcon::Loading, _T ("wait.png") }
	};
	if (StatusIcon::Loading == _icon) {
		m_img_status->SetAnimateBkImage (_status_images[_icon], 112, 14);
	} else {
		m_img_status->SetBkImage (_status_images[_icon]);
	}
	m_text_status->SetText (_info);
}

void NetToolboxWnd::show_error_code (DWORD err_no) {
	if (err_no == 0) {
		show_status (StatusIcon::Ok, _T ("成功。"));
	} else {
		show_status (StatusIcon::Ok, tool_Utils::get_error_info (err_no).data ());
	}
}

void NetToolboxWnd::direct_page (size_t sel1, size_t sel2) {
	BindOptionUI { std::format (_T ("group{}"), sel1) }->Activate ();
	BindOptionUI { std::format (_T ("tab{}_{}"), sel1, sel2) }->Activate ();
}

bool NetToolboxWnd::enum_pages (std::function<bool (page_base*)> f) {
	for (size_t i = 0; i < m_pages.size (); ++i) {
		for (size_t j = 0; j < m_pages[i].size (); ++j) {
			if (m_pages[i][j])
				if (f (m_pages[i][j]))
					return true;
		}
	}
	return false;
}



void NetToolboxWnd::ui_update_data () {
	if (m_pages.size () > m_sel1 && m_pages[m_sel1].size () > m_sel2 && m_pages[m_sel1][m_sel2])
		m_pages[m_sel1][m_sel2]->ui_update_data ();
	::DragAcceptFiles (m_hWnd, m_sel1 == 5 && m_sel2 == 3);
}
