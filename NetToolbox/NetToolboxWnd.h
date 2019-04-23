#ifndef __NET_TOOLBOX_WND_HPP__
#define __NET_TOOLBOX_WND_HPP__

#include "resource.h"

#include "../DuiLib/StdAfx.h"
#include "../DuiLib/UIlib.h"
using namespace DuiLib;

#include <vector>
#include <functional>
#include <thread>

#include "HanAnim.hpp"
#include "International.hpp"

#include "pages/page_base.hpp"



class NetToolboxWnd: public WindowImplBase {
public:
	enum class StatusIcon {
		Ok			= 0,
		Info		= 1,
		Warning		= 2,
		Error		= 3,
		Loading		= 4
	};
	NetToolboxWnd (faw::String caption): m_caption (caption) {}

	// duilib所需要的重载函数
	faw::string_view_t GetWindowClassName () const override { return _T ("NetToolbox"); }
	faw::string_view_t GetSkinFile () override { return International::translate (_T ("main.xml")); }
	void InitWindow () override;
	void OnClick (TNotifyUI& msg) override;
	void OnHeaderClick (TNotifyUI& msg) override;
	void OnSelectChanged (TNotifyUI& msg) override;
	void OnTextChanged (TNotifyUI& msg) override;
	void OnItemSelect (TNotifyUI& msg) override;
	void OnTimer (TNotifyUI& msg) override;
	void Notify (TNotifyUI& msg) override;
	virtual void OnDropFiles (HDROP hDrop);
	LRESULT OnLButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	LRESULT OnLButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	LRESULT OnRButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	LRESULT OnRButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	CControlUI* CreateControl (faw::string_view_t pstrClass) override;

	// 窗口所提供的功能函数
	void show_status (StatusIcon _icon = StatusIcon::Info, faw::String _info = _T (""));
	void show_error_code (DWORD last_error);
	LRESULT invoke (std::function<LRESULT ()> f) { return ::SendMessage (m_hWnd, WM_USER + 0x101, 1, (LPARAM) &f); }
	void async_invoke (std::function<LRESULT ()> f) { ::PostMessage (m_hWnd, WM_USER + 0x101, 0, (LPARAM) new decltype (f) (f)); }
	void direct_page (size_t sel1, size_t sel2 = faw::String::_npos);
	CControlUI *find_control (POINT pt) { return m_pm.FindControl (pt); }
	HINSTANCE get_instance () { return CPaintManagerUI::GetInstance (); }
	CPaintManagerUI *get_pm () { return &m_pm; }
	page_base *get_moment_page () { return (m_pages.size () > m_sel1 && m_pages[m_sel1].size () > m_sel2 ? m_pages[m_sel1][m_sel2] : nullptr); }

	bool enum_pages (std::function<bool (page_base*)> f);

	size_t									m_sel1 = 0, m_sel2 = 0;
private:
	// 功能处理函数
	void ui_update_data ();

	faw::String								m_caption;
	HICON			m_icon				= ::LoadIcon (CPaintManagerUI::GetInstance (), MAKEINTRESOURCE (IDI_ICON1));
	DWORD									m_last_img_status	= 1;
	BindTextUI								m_text_caption { _T ("text_caption2") };
	BindHanAnimUI							m_img_status { _T ("img_status") };
	BindTextUI								m_text_status { _T ("text_status") };
	BindTabLayoutUI							m_tabM { _T ("tabM") };
	std::vector<BindTabLayoutUI>			m_tab { { _T ("tab0") }, { _T ("tab1") }, { _T ("tab2") }, { _T ("tab3") }, { _T ("tab4") }, { _T ("tab5") }, { _T ("tab6") } };
	std::vector<std::vector<page_base*>>	m_pages;
};

#endif //__NET_TOOLBOX_WND_HPP__
