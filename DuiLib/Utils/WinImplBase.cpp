#include "StdAfx.h"
#include <algorithm>
namespace DuiLib {
	//////////////////////////////////////////////////////////////////////////
	//
	DUI_BEGIN_MESSAGE_MAP (WindowImplBase, CNotifyPump)
		DUI_ON_MSGTYPE (DUI_MSGTYPE_CLICK, OnClick)
		DUI_ON_MSGTYPE (DUI_MSGTYPE_HEADERCLICK, OnHeaderClick)
		DUI_ON_MSGTYPE (DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
		DUI_ON_MSGTYPE (DUI_MSGTYPE_TEXTCHANGED, OnTextChanged)
		DUI_ON_MSGTYPE (DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
		DUI_ON_MSGTYPE (DUI_MSGTYPE_TIMER, OnTimer)
	DUI_END_MESSAGE_MAP ()

	void WindowImplBase::OnFinalMessage (HWND hWnd) {
		m_pm.RemovePreMessageFilter (this);
		m_pm.RemoveNotifier (this);
		m_pm.ReapObjects (m_pm.GetRoot ());
	}

	LRESULT WindowImplBase::ResponseDefaultKeyEvent (WPARAM wParam) {
		if (wParam == VK_RETURN) {
			return FALSE;
		} else if (wParam == VK_ESCAPE) {
			return TRUE;
		}

		return FALSE;
	}

	UINT WindowImplBase::GetClassStyle () const {
		return CS_DBLCLKS;
	}

	CControlUI* WindowImplBase::CreateControl (string_view_t pstrClass) {
		return nullptr;
	}

	string_view_t WindowImplBase::QueryControlText (string_view_t lpstrId, string_view_t lpstrType) {
		return nullptr;
	}

	LRESULT WindowImplBase::MessageHandler (UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/) {
		if (uMsg == WM_KEYDOWN) {
			switch (wParam) {
			case VK_RETURN:
			case VK_ESCAPE:
				return ResponseDefaultKeyEvent (wParam);
			default:
				break;
			}
		}
		return FALSE;
	}

	LRESULT WindowImplBase::OnClose (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnDestroy (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

#if defined(WIN32) && !defined(UNDER_CE)
	LRESULT WindowImplBase::OnNcActivate (UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled) {
		if (::IsIconic (GetHWND ())) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT WindowImplBase::OnNcCalcSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		return 0;
	}

	LRESULT WindowImplBase::OnNcPaint (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		return 0;
	}


	BOOL WindowImplBase::IsInStaticControl (CControlUI *pControl, POINT &pt) {
		if (!pControl || pControl->IsDynamic (pt))
			return FALSE;

		CDuiString strClassName;
		static std::vector<CDuiString> vctStaticName { _T ("controlui"), _T ("textui"), _T ("labelui"), _T ("containerui"), _T ("horizontallayoutui"), _T ("verticallayoutui"), _T("tablayoutui"), _T ("childlayoutui"), _T ("dialoglayoutui"), _T ("progresscontainerui") };

		strClassName = pControl->GetClass ();
		strClassName.MakeLower ();
		if (vctStaticName.end () != std::find (vctStaticName.begin (), vctStaticName.end (), strClassName)) {
			CControlUI* pParent = pControl->GetParent ();
			while (pParent) {
				if (pParent->IsDynamic (pt))
					return FALSE;
				strClassName = pParent->GetClass ();
				strClassName.MakeLower ();
				if (vctStaticName.end () == std::find (vctStaticName.begin (), vctStaticName.end (), strClassName))
					return FALSE;

				pParent = pParent->GetParent ();
			}

			return TRUE;
		}

		return FALSE;
	}

	LRESULT WindowImplBase::OnNcHitTest (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		POINT pt { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
		::ScreenToClient (GetHWND (), &pt);

		RECT rcClient = { 0 };
		::GetClientRect (GetHWND (), &rcClient);

		if (!::IsZoomed (GetHWND ())) {
			RECT rcSizeBox = m_pm.GetSizeBox ();
			if (pt.y < rcClient.top + rcSizeBox.top) {
				if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
				if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
				return HTTOP;
			} else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
				if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
				if (pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}

			if (pt.x < rcClient.left + rcSizeBox.left) return HTLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
		}

		RECT rcCaption = m_pm.GetCaptionRect ();
		if (-1 == rcCaption.bottom) {
			rcCaption.bottom = rcClient.bottom;
		}

		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = m_pm.FindControl (pt);
			if (IsInStaticControl (pControl, pt)) {
				return HTCAPTION;
			}
		}

		return HTCLIENT;
	}

	LRESULT WindowImplBase::OnGetMinMaxInfo (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		MONITORINFO Monitor = {};
		Monitor.cbSize = sizeof (Monitor);
		::GetMonitorInfo (::MonitorFromWindow (m_hWnd, MONITOR_DEFAULTTOPRIMARY), &Monitor);
		RECT rcWork = Monitor.rcWork;
		if (Monitor.dwFlags != MONITORINFOF_PRIMARY) {
			::OffsetRect (&rcWork, -rcWork.left, -rcWork.top);
		}

		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
		lpMMI->ptMaxPosition.x = rcWork.left;
		lpMMI->ptMaxPosition.y = rcWork.top;
		lpMMI->ptMaxSize.x = rcWork.right - rcWork.left;
		lpMMI->ptMaxSize.y = rcWork.bottom - rcWork.top;
		lpMMI->ptMaxTrackSize.x = m_pm.GetMaxInfo ().cx == 0 ? rcWork.right - rcWork.left : m_pm.GetMaxInfo ().cx;
		lpMMI->ptMaxTrackSize.y = m_pm.GetMaxInfo ().cy == 0 ? rcWork.bottom - rcWork.top : m_pm.GetMaxInfo ().cy;
		lpMMI->ptMinTrackSize.x = m_pm.GetMinInfo ().cx;
		lpMMI->ptMinTrackSize.y = m_pm.GetMinInfo ().cy;

		bHandled = TRUE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseWheel (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseHover (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}
#endif

	LRESULT WindowImplBase::OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		SIZE szRoundCorner = m_pm.GetRoundCorner ();
#if defined(WIN32) && !defined(UNDER_CE)
		if (!::IsIconic (GetHWND ())) {
			RECT rcWnd = { 0 };
			::GetWindowRect (GetHWND (), &rcWnd);
			::OffsetRect (&rcWnd, -rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn (rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn (GetHWND (), hRgn, TRUE);
			::DeleteObject (hRgn);
		}
#endif
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnChar (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnSysCommand (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		if (wParam == SC_CLOSE) {
			bHandled = TRUE;
			SendMessage (WM_CLOSE);
			return 0;
		}
#if defined(WIN32) && !defined(UNDER_CE)
		BOOL bZoomed = ::IsZoomed (GetHWND ());
		LRESULT lRes = CWindowWnd::HandleMessage (uMsg, wParam, lParam);
		if (::IsZoomed (GetHWND ()) != bZoomed) {
			if (!bZoomed) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl (_T ("maxbtn")));
				if (pControl) pControl->SetVisible (false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl (_T ("restorebtn")));
				if (pControl) pControl->SetVisible (true);
			} else {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl (_T ("maxbtn")));
				if (pControl) pControl->SetVisible (true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl (_T ("restorebtn")));
				if (pControl) pControl->SetVisible (false);
			}
		}
#else
		LRESULT lRes = CWindowWnd::HandleMessage (uMsg, wParam, lParam);
#endif
		return lRes;
	}

	LRESULT WindowImplBase::OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		// 调整窗口样式
		LONG styleValue = ::GetWindowLong (GetHWND (), GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong (GetHWND (), GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		// 关联UI管理器
		m_pm.Init (m_hWnd, GetManagerName ());
		// 注册PreMessage回调
		m_pm.AddPreMessageFilter (this);

		// 创建主窗口
		CControlUI* pRoot = nullptr;
		CDialogBuilder builder;
		CDuiString sSkinType = GetSkinType ();
		std::variant<UINT, string_t> xml;
		if (!sSkinType.empty ()) {
			std::variant<UINT, string_t> xml = FawTools::parse_dec (GetSkinFile ());
			pRoot = builder.Create (xml, sSkinType, this, &m_pm);
		} else {
			std::variant<UINT, string_t> xml = string_t (GetSkinFile ());
			pRoot = builder.Create (xml, _T (""), this, &m_pm);
		}

		if (!pRoot) {
			CDuiString sError = _T ("加载资源文件失败：");
			sError += GetSkinFile ();
			MessageBox (nullptr, sError.c_str (), _T ("Duilib"), MB_OK | MB_ICONERROR);
			ExitProcess (1);
			return 0;
		}
		m_pm.AttachDialog (pRoot);
		// 添加Notify事件接口
		m_pm.AddNotifier (this);
		// 开始绑定控件
		//BindCtrlBase::init_binding (&m_pm);
		// 窗口初始化完毕
		InitWindow ();
		return 0;
	}

	LRESULT WindowImplBase::OnKeyDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnKillFocus (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnSetFocus (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonUp (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnRButtonDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnRButtonUp (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseMove (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam) {
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_CREATE:			lRes = OnCreate (uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose (uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy (uMsg, wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)
		case WM_NCACTIVATE:		lRes = OnNcActivate (uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize (uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint (uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest (uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo (uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEWHEEL:		lRes = OnMouseWheel (uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:			lRes = OnSize (uMsg, wParam, lParam, bHandled); break;
		case WM_CHAR:			lRes = OnChar (uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:		lRes = OnSysCommand (uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:		lRes = OnKeyDown (uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:		lRes = OnKillFocus (uMsg, wParam, lParam, bHandled); break;
		case WM_SETFOCUS:		lRes = OnSetFocus (uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONUP:		lRes = OnLButtonUp (uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONDOWN:	lRes = OnLButtonDown (uMsg, wParam, lParam, bHandled); break;
		case WM_RBUTTONUP:		lRes = OnRButtonUp (uMsg, wParam, lParam, bHandled); break;
		case WM_RBUTTONDOWN:	lRes = OnRButtonDown (uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEMOVE:		lRes = OnMouseMove (uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEHOVER:		lRes = OnMouseHover (uMsg, wParam, lParam, bHandled); break;
		default:				bHandled = FALSE; break;
		}
		if (bHandled) return lRes;

		lRes = HandleCustomMessage (uMsg, wParam, lParam, bHandled);
		if (bHandled) return lRes;

		if (m_pm.MessageHandler (uMsg, wParam, lParam, lRes))
			return lRes;
		return CWindowWnd::HandleMessage (uMsg, wParam, lParam);
	}

	LRESULT WindowImplBase::HandleCustomMessage (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bHandled = FALSE;
		return 0;
	}

	LONG WindowImplBase::GetStyle () {
		LONG styleValue = ::GetWindowLong (GetHWND (), GWL_STYLE);
		styleValue &= ~WS_CAPTION;

		return styleValue;
	}

	void WindowImplBase::OnClick (TNotifyUI& msg) {
		CDuiString sCtrlName = msg.pSender->GetName ();
		if (sCtrlName == _T ("closebtn")) {
			Close ();
			return;
		} else if (sCtrlName == _T ("minbtn")) {
			SendMessage (WM_SYSCOMMAND, SC_MINIMIZE, 0);
			return;
		} else if (sCtrlName == _T ("maxbtn")) {
			SendMessage (WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			return;
		} else if (sCtrlName == _T ("restorebtn")) {
			SendMessage (WM_SYSCOMMAND, SC_RESTORE, 0);
			return;
		}
		return;
	}

	void WindowImplBase::Notify (TNotifyUI& msg) {
		return CNotifyPump::NotifyPump (msg);
	}
}