#include "StdAfx.h"

#include "UIMenu.h"

namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL (CMenuUI)

	CMenuUI::CMenuUI () {
		if (GetHeader ())
			GetHeader ()->SetVisible (false);
	}

	CMenuUI::~CMenuUI () {}

	faw::string_view_t CMenuUI::GetClass () const {
		return _T ("MenuUI");
	}

	LPVOID CMenuUI::GetInterface (faw::string_view_t pstrName) {
		if (pstrName == _T ("Menu")) return static_cast<CMenuUI*>(this);
		return CListUI::GetInterface (pstrName);
	}

	UINT CMenuUI::GetListType () {
		return LT_MENU;
	}

	void CMenuUI::DoEvent (TEventUI& event) {
		return __super::DoEvent (event);
	}

	bool CMenuUI::Add (CControlUI* pControl) {
		CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface (_T ("MenuElement")));
		if (!pMenuItem)
			return false;

		for (int i = 0; i < pMenuItem->GetCount (); ++i) {
			if (pMenuItem->GetItemAt (i)->GetInterface (_T ("MenuElement"))) {
				(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetInternVisible (false);
			}
		}
		return CListUI::Add (pControl);
	}

	bool CMenuUI::AddAt (CControlUI* pControl, int iIndex) {
		CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface (_T ("MenuElement")));
		if (!pMenuItem)
			return false;

		for (int i = 0; i < pMenuItem->GetCount (); ++i) {
			if (pMenuItem->GetItemAt (i)->GetInterface (_T ("MenuElement"))) {
				(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetInternVisible (false);
			}
		}
		return CListUI::AddAt (pControl, iIndex);
	}

	int CMenuUI::GetItemIndex (CControlUI* pControl) const {
		CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface (_T ("MenuElement")));
		if (!pMenuItem)
			return -1;

		return __super::GetItemIndex (pControl);
	}

	bool CMenuUI::SetItemIndex (CControlUI* pControl, int iIndex) {
		CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface (_T ("MenuElement")));
		if (!pMenuItem)
			return false;

		return __super::SetItemIndex (pControl, iIndex);
	}

	bool CMenuUI::Remove (CControlUI* pControl) {
		CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface (_T ("MenuElement")));
		if (!pMenuItem)
			return false;

		return __super::Remove (pControl);
	}

	SIZE CMenuUI::EstimateSize (SIZE szAvailable) {
		int cxFixed = 0;
		int cyFixed = 0;
		for (int it = 0; it < GetCount (); it++) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt (it));
			if (!pControl->IsVisible ()) continue;
			SIZE sz = pControl->EstimateSize (szAvailable);
			cyFixed += sz.cy;
			if (cxFixed < sz.cx)
				cxFixed = sz.cx;
		}

		for (int it = 0; it < GetCount (); it++) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt (it));
			if (!pControl->IsVisible ()) continue;

			pControl->SetFixedWidth (MulDiv (cxFixed, 100, GetManager ()->GetDPIObj ()->GetScale ()));
		}

		return { cxFixed, cyFixed };
	}

	void CMenuUI::SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue) {
		CListUI::SetAttribute (pstrName, pstrValue);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//

	CMenuWnd::CMenuWnd ():
		m_pOwner (nullptr),
		m_pLayout (),
		m_xml (_T ("")),
		isClosing (false) {
		m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
	}

	CMenuWnd::~CMenuWnd () {

	}

	void CMenuWnd::Close (UINT nRet) {
		//ASSERT (::IsWindow (m_hWnd));
		if (!::IsWindow (m_hWnd)) return;
		PostMessage (WM_CLOSE, (WPARAM) nRet, 0L);
		isClosing = true;
	}


	BOOL CMenuWnd::Receive (ContextMenuParam param) {
		switch (param.wParam) {
		case 1:
			Close ();
			break;
		case 2:
		{
			HWND hParent = GetParent (m_hWnd);
			while (hParent) {
				if (hParent == param.hWnd) {
					Close ();
					break;
				}
				hParent = GetParent (hParent);
			}
		}
		break;
		default:
			break;
		}

		return TRUE;
	}

	CMenuWnd* CMenuWnd::CreateMenu (CMenuElementUI* pOwner, std::variant<UINT, faw::String> xml, POINT point, CPaintManagerUI* pMainPaintManager, CStdStringPtrMap* pMenuCheckInfo /*= nullptr*/, DWORD dwAlignment /*= eMenuAlignment_Left | eMenuAlignment_Top*/) {
		CMenuWnd* pMenu = new CMenuWnd;
		pMenu->Init (pOwner, xml, point, pMainPaintManager, pMenuCheckInfo, dwAlignment);
		return pMenu;
	}

	void CMenuWnd::DestroyMenu () {
		CStdStringPtrMap* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver ().GetMenuCheckInfo ();
		if (mCheckInfos) {
			for (int i = 0; i < mCheckInfos->GetSize (); i++) {
				MenuItemInfo* pItemInfo = (MenuItemInfo*) mCheckInfos->GetAt (i)->Data;
				if (pItemInfo) {
					delete pItemInfo;
					pItemInfo = nullptr;
				}
			}
			mCheckInfos->Resize (0);
		}
	}

	MenuItemInfo* CMenuWnd::SetMenuItemInfo (faw::string_view_t pstrName, bool bChecked) {
		if (pstrName.empty ()) return nullptr;

		CStdStringPtrMap* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver ().GetMenuCheckInfo ();
		if (mCheckInfos) {
			MenuItemInfo* pItemInfo = (MenuItemInfo*) mCheckInfos->Find (pstrName);
			if (!pItemInfo) {
				pItemInfo = new MenuItemInfo;
				pItemInfo->szName = pstrName;
				pItemInfo->bChecked = bChecked;
				mCheckInfos->Insert (pstrName, pItemInfo);
			} else {
				pItemInfo->bChecked = bChecked;
			}

			return pItemInfo;
		}
		return nullptr;
	}

	void CMenuWnd::Init (CMenuElementUI* pOwner, std::variant<UINT, faw::String> xml, POINT point,
		CPaintManagerUI* pMainPaintManager, CStdStringPtrMap* pMenuCheckInfo/* = nullptr*/,
		DWORD dwAlignment/* = eMenuAlignment_Left | eMenuAlignment_Top*/) {

		m_BasedPoint = point;
		m_pOwner = pOwner;
		m_pLayout = nullptr;
		m_xml = xml;
		m_dwAlignment = dwAlignment;

		// 如果是一级菜单的创建
		if (!pOwner) {
			ASSERT (pMainPaintManager);
			CMenuWnd::GetGlobalContextMenuObserver ().SetManger (pMainPaintManager);
			if (pMenuCheckInfo)
				CMenuWnd::GetGlobalContextMenuObserver ().SetMenuCheckInfo (pMenuCheckInfo);
		}

		CMenuWnd::GetGlobalContextMenuObserver ().AddReceiver (this);

		Create (!m_pOwner ? pMainPaintManager->GetPaintWindow () : m_pOwner->GetManager ()->GetPaintWindow (), _T (""), WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, { 0, 0, 0, 0 });

		// HACK: Don't deselect the parent's caption
		HWND hWndParent = m_hWnd;
		while (::GetParent (hWndParent)) hWndParent = ::GetParent (hWndParent);

		::ShowWindow (m_hWnd, SW_SHOW);
		::SendMessage (hWndParent, WM_NCACTIVATE, TRUE, 0L);
	}

	faw::string_view_t CMenuWnd::GetWindowClassName () const {
		return _T ("DuiMenuWnd");
	}


	void CMenuWnd::Notify (TNotifyUI& msg) {
		if (CMenuWnd::GetGlobalContextMenuObserver ().GetManager ()) {
			if (msg.sType == _T ("click") || msg.sType == _T ("valuechanged")) {
				CMenuWnd::GetGlobalContextMenuObserver ().GetManager ()->SendNotify (msg, false);
			}
		}

	}

	CControlUI* CMenuWnd::CreateControl (faw::string_view_t pstrClassName) {
		if (pstrClassName == _T ("Menu")) {
			return new CMenuUI ();
		} else if (pstrClassName == _T ("MenuElement")) {
			return new CMenuElementUI ();
		}
		return nullptr;
	}


	void CMenuWnd::OnFinalMessage (HWND hWnd) {
		RemoveObserver ();
		if (m_pOwner) {
			for (int i = 0; i < m_pOwner->GetCount (); i++) {
				if (static_cast<CMenuElementUI*>(m_pOwner->GetItemAt (i)->GetInterface (_T ("MenuElement")))) {
					(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt (i)))->SetOwner (m_pOwner->GetParent ());
					(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt (i)))->SetVisible (false);
					(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetInternVisible (false);
				}
			}
			m_pOwner->m_pWindow = nullptr;
			m_pOwner->m_uButtonState &= ~UISTATE_PUSHED;
			m_pOwner->Invalidate ();

			// 内部创建的内部删除
			delete this;
		}
	}

	LRESULT CMenuWnd::OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		bool bShowShadow = false;
		if (m_pOwner) {
			LONG styleValue = ::GetWindowLong (GetHWND (), GWL_STYLE);
			styleValue &= ~WS_CAPTION;
			::SetWindowLong (GetHWND (), GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			RECT rcClient = { 0 };
			::GetClientRect (GetHWND (), &rcClient);
			::SetWindowPos (GetHWND (), nullptr, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
				rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

			m_pm.Init (m_hWnd);
			m_pm.GetDPIObj ()->SetScale (m_pOwner->GetManager ()->GetDPIObj ()->GetDPI ());
			// The trick is to add the items to the new container. Their owner gets
			// reassigned by this operation - which is why it is important to reassign
			// the items back to the righfull owner/manager when the window closes.
			m_pLayout = new CMenuUI ();
			m_pm.SetForceUseSharedRes (true);
			m_pLayout->SetManager (&m_pm, nullptr, true);
			faw::string_view_t pDefaultAttributes = m_pOwner->GetManager ()->GetDefaultAttributeList (_T ("Menu"));
			if (!pDefaultAttributes.empty ()) {
				m_pLayout->ApplyAttributeList (pDefaultAttributes);
			}
			m_pLayout->GetList ()->SetAutoDestroy (false);

			for (int i = 0; i < m_pOwner->GetCount (); i++) {
				if (m_pOwner->GetItemAt (i)->GetInterface (_T ("MenuElement"))) {
					(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt (i)))->SetOwner (m_pLayout);
					m_pLayout->Add (static_cast<CControlUI*>(m_pOwner->GetItemAt (i)));
				}
			}

			CShadowUI *pShadow = m_pOwner->GetManager ()->GetShadow ();
			pShadow->CopyShadow (m_pm.GetShadow ());
			bShowShadow = m_pm.GetShadow ()->IsShowShadow ();
			m_pm.GetShadow ()->ShowShadow (false);
			m_pm.SetLayered (m_pOwner->GetManager ()->IsLayered ());
			m_pm.AttachDialog (m_pLayout);
			m_pm.AddNotifier (this);

			ResizeSubMenu ();
		} else {
			m_pm.Init (m_hWnd);
			m_pm.GetDPIObj ()->SetScale (CMenuWnd::GetGlobalContextMenuObserver ().GetManager ()->GetDPIObj ()->GetDPI ());
			CDialogBuilder builder;

			CControlUI* pRoot = builder.Create (m_xml, _T (""), this, &m_pm);
			bShowShadow = m_pm.GetShadow ()->IsShowShadow ();
			m_pm.GetShadow ()->ShowShadow (false);
			m_pm.AttachDialog (pRoot);
			m_pm.AddNotifier (this);

			ResizeMenu ();
		}
		GetMenuUI ()->m_pWindow = this;
		m_pm.GetShadow ()->ShowShadow (bShowShadow);
		m_pm.GetShadow ()->Create (&m_pm);
		return 0;
	}

	CMenuUI* CMenuWnd::GetMenuUI () {
		return static_cast<CMenuUI*>(m_pm.GetRoot ());
	}

	void CMenuWnd::ResizeMenu () {
		CControlUI* pRoot = m_pm.GetRoot ();

#if defined(WIN32) && !defined(UNDER_CE)
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof (oMonitor);
		::GetMonitorInfo (::MonitorFromWindow (GetHWND (), MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		RECT rcWork = oMonitor.rcWork;
#else
		RECT rcWork = { 0 };
		GetWindowRect (m_pOwner->GetManager ()->GetPaintWindow (), &rcWork);
#endif
		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
		szAvailable = pRoot->EstimateSize (szAvailable);
		m_pm.SetInitSize (szAvailable.cx, szAvailable.cy);

		//必须是Menu标签作为xml的根节点
		CMenuUI *pMenuRoot = static_cast<CMenuUI*>(pRoot);
		ASSERT (pMenuRoot);

		SIZE szInit = m_pm.GetInitSize ();
		RECT rc = { 0 };
		POINT point = m_BasedPoint;
		rc.left = point.x;
		rc.top = point.y;
		rc.right = rc.left + szInit.cx;
		rc.bottom = rc.top + szInit.cy;

		int nWidth = rc.right - rc.left;
		int nHeight = rc.bottom - rc.top;

		if (m_dwAlignment & eMenuAlignment_Right) {
			rc.right = point.x;
			rc.left = rc.right - nWidth;
		}

		if (m_dwAlignment & eMenuAlignment_Bottom) {
			rc.bottom = point.y;
			rc.top = rc.bottom - nHeight;
		}

		SetForegroundWindow (m_hWnd);
		MoveWindow (m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
		SetWindowPos (m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + pMenuRoot->GetInset ().bottom + pMenuRoot->GetInset ().top, SWP_SHOWWINDOW);
	}

	void CMenuWnd::ResizeSubMenu () {
		// Position the popup window in absolute space
		RECT rcOwner = m_pOwner->GetPos ();
		RECT rc = rcOwner;

		int cxFixed = 0;
		int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof (oMonitor);
		::GetMonitorInfo (::MonitorFromWindow (GetHWND (), MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		RECT rcWork = oMonitor.rcWork;
#else
		RECT rcWork = { 0 };
		GetWindowRect (m_pOwner->GetManager ()->GetPaintWindow (), &rcWork);
#endif
		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

		for (int it = 0; it < m_pOwner->GetCount (); it++) {
			if (m_pOwner->GetItemAt (it)->GetInterface (_T ("MenuElement"))) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt (it));
				SIZE sz = pControl->EstimateSize (szAvailable);
				cyFixed += sz.cy;
				if (cxFixed < sz.cx) cxFixed = sz.cx;
			}
		}

		RECT rcWindow = { 0 };
		GetWindowRect (m_pOwner->GetManager ()->GetPaintWindow (), &rcWindow);

		rc.top = rcOwner.top;
		rc.bottom = rc.top + cyFixed;
		::MapWindowRect (m_pOwner->GetManager ()->GetPaintWindow (), HWND_DESKTOP, &rc);
		rc.left = rcWindow.right;
		rc.right = rc.left + cxFixed;
		rc.right += 2;

		bool bReachBottom = false;
		bool bReachRight = false;
		LONG chRightAlgin = 0;
		LONG chBottomAlgin = 0;

		RECT rcPreWindow = { 0 };
		MenuObserverImpl::Iterator iterator (CMenuWnd::GetGlobalContextMenuObserver ());
		MenuMenuReceiverImplBase* pReceiver = iterator.next ();
		while (pReceiver) {
			CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
			if (pContextMenu) {
				GetWindowRect (pContextMenu->GetHWND (), &rcPreWindow);

				bReachRight = rcPreWindow.left >= rcWindow.right;
				bReachBottom = rcPreWindow.top >= rcWindow.bottom;
				if (pContextMenu->GetHWND () == m_pOwner->GetManager ()->GetPaintWindow () || bReachBottom || bReachRight)
					break;
			}
			pReceiver = iterator.next ();
		}

		if (bReachBottom) {
			rc.bottom = rcWindow.top;
			rc.top = rc.bottom - cyFixed;
		}

		if (bReachRight) {
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}

		if (rc.bottom > rcWork.bottom) {
			rc.bottom = rc.top;
			rc.top = rc.bottom - cyFixed;
		}

		if (rc.right > rcWork.right) {
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}

		if (rc.top < rcWork.top) {
			rc.top = rcOwner.top;
			rc.bottom = rc.top + cyFixed;
		}

		if (rc.left < rcWork.left) {
			rc.left = rcWindow.right;
			rc.right = rc.left + cxFixed;
		}

		MoveWindow (m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + m_pLayout->GetInset ().top + m_pLayout->GetInset ().bottom, FALSE);
	}

	void CMenuWnd::setDPI (int DPI) {
		m_pm.SetDPI (DPI);
	}


	LRESULT CMenuWnd::OnKillFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		HWND hFocusWnd = (HWND) wParam;

		BOOL bInMenuWindowList = FALSE;
		ContextMenuParam param;
		param.hWnd = GetHWND ();

		MenuObserverImpl::Iterator iterator (CMenuWnd::GetGlobalContextMenuObserver ());
		MenuMenuReceiverImplBase* pReceiver = iterator.next ();
		while (pReceiver) {
			CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
			if (pContextMenu && pContextMenu->GetHWND () == hFocusWnd) {
				bInMenuWindowList = TRUE;
				break;
			}
			pReceiver = iterator.next ();
		}

		if (!bInMenuWindowList) {
			param.wParam = 1;
			CMenuWnd::GetGlobalContextMenuObserver ().RBroadcast (param);
			return 0;
		}
		return 0;
	}
	LRESULT CMenuWnd::OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		SIZE szRoundCorner = m_pm.GetRoundCorner ();
		if (!::IsIconic (GetHWND ())) {
			RECT rcWnd = { 0 };
			::GetWindowRect (GetHWND (), &rcWnd);
			::OffsetRect (&rcWnd, -rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn (rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn (GetHWND (), hRgn, TRUE);
			::DeleteObject (hRgn);
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT CMenuWnd::HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam) {
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_CREATE:
			lRes = OnCreate (uMsg, wParam, lParam, bHandled);
			break;
		case WM_KILLFOCUS:
			lRes = OnKillFocus (uMsg, wParam, lParam, bHandled);
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE || wParam == VK_LEFT)
				Close ();
			break;
		case WM_SIZE:
			lRes = OnSize (uMsg, wParam, lParam, bHandled);
			break;
		case WM_CLOSE:
			if (m_pOwner) {
				m_pOwner->SetManager (m_pOwner->GetManager (), m_pOwner->GetParent (), false);
				m_pOwner->SetPos (m_pOwner->GetPos ());
				m_pOwner->SetFocus ();
			}
			break;
		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
			return 0L;
			break;
		default:
			bHandled = FALSE;
			break;
		}

		if (m_pm.MessageHandler (uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage (uMsg, wParam, lParam);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL (CMenuElementUI)

	CMenuElementUI::CMenuElementUI (): m_dwLineColor ((DWORD) DEFAULT_LINE_COLOR) {
		m_cxyFixed.cy = ITEM_DEFAULT_HEIGHT;
		m_cxyFixed.cx = ITEM_DEFAULT_WIDTH;
		m_szIconSize.cy = ITEM_DEFAULT_ICON_SIZE;
		m_szIconSize.cx = ITEM_DEFAULT_ICON_SIZE;

		m_rcLinePadding.top = m_rcLinePadding.bottom = 0;
		m_rcLinePadding.left = DEFAULT_LINE_LEFT_INSET;
		m_rcLinePadding.right = DEFAULT_LINE_RIGHT_INSET;
	}

	CMenuElementUI::~CMenuElementUI () {}

	faw::string_view_t CMenuElementUI::GetClass () const {
		return _T ("MenuElementUI");
	}

	LPVOID CMenuElementUI::GetInterface (faw::string_view_t pstrName) {
		if (pstrName == _T ("MenuElement")) return static_cast<CMenuElementUI*>(this);
		return CListContainerElementUI::GetInterface (pstrName);
	}

	bool CMenuElementUI::DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
		SIZE _cxyFixed = CMenuElementUI::m_cxyFixed;
		_cxyFixed.cx = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cx);
		_cxyFixed.cy = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cy);
		RECT _rcLinePadding = CMenuElementUI::m_rcLinePadding;
		GetManager ()->GetDPIObj ()->Scale (&_rcLinePadding);

		RECT rcTemp = { 0 };
		if (!::IntersectRect (&rcTemp, &rcPaint, &m_rcItem)) return true;

		if (m_bDrawLine) {
			RECT rcLine = { m_rcItem.left + _rcLinePadding.left, m_rcItem.top + _cxyFixed.cy / 2, m_rcItem.right - _rcLinePadding.right, m_rcItem.top + _cxyFixed.cy / 2 };
			CRenderEngine::DrawLine (hDC, rcLine, 1, m_dwLineColor);
		} else {
			//CMenuElementUI::DrawItemBk(hDC, m_rcItem);
			//DrawItemText(hDC, m_rcItem);
			//DrawItemIcon(hDC, m_rcItem);
			//DrawItemExpland(hDC, m_rcItem);
			//for (int i = 0; i < GetCount(); ++i)
			//{
			//	if (!GetItemAt(i)->GetInterface(_T("MenuElement"))) {
			//		GetItemAt(i)->DoPaint(hDC, rcPaint);
			//	}
			//}

			CRenderClip clip;
			CRenderClip::GenerateClip (hDC, rcTemp, clip);
			CMenuElementUI::DrawItemBk (hDC, m_rcItem);
			DrawItemText (hDC, m_rcItem);
			DrawItemIcon (hDC, m_rcItem);
			DrawItemExpland (hDC, m_rcItem);

			if (m_items.GetSize () > 0) {
				RECT rc = m_rcItem;
				rc.left += m_rcInset.left;
				rc.top += m_rcInset.top;
				rc.right -= m_rcInset.right;
				rc.bottom -= m_rcInset.bottom;
				if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) rc.right -= m_pVerticalScrollBar->GetFixedWidth ();
				if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight ();

				if (!::IntersectRect (&rcTemp, &rcPaint, &rc)) {
					for (int it = 0; it < m_items.GetSize (); it++) {
						CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
						if (pControl == pStopControl) return false;
						if (!pControl->IsVisible ()) continue;
						if (pControl->GetInterface (_T ("MenuElement"))) continue;
						if (!::IntersectRect (&rcTemp, &rcPaint, &pControl->GetPos ())) continue;
						if (pControl->IsFloat ()) {
							if (!::IntersectRect (&rcTemp, &m_rcItem, &pControl->GetPos ())) continue;
							if (!pControl->Paint (hDC, rcPaint, pStopControl)) return false;
						}
					}
				} else {
					CRenderClip childClip;
					CRenderClip::GenerateClip (hDC, rcTemp, childClip);
					for (int it = 0; it < m_items.GetSize (); it++) {
						CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
						if (pControl == pStopControl) return false;
						if (!pControl->IsVisible ()) continue;
						if (pControl->GetInterface (_T ("MenuElement"))) continue;
						if (!::IntersectRect (&rcTemp, &rcPaint, &pControl->GetPos ())) continue;
						if (pControl->IsFloat ()) {
							if (!::IntersectRect (&rcTemp, &m_rcItem, &pControl->GetPos ())) continue;
							CRenderClip::UseOldClipBegin (hDC, childClip);
							if (!pControl->Paint (hDC, rcPaint, pStopControl)) return false;
							CRenderClip::UseOldClipEnd (hDC, childClip);
						} else {
							if (!::IntersectRect (&rcTemp, &rc, &pControl->GetPos ())) continue;
							if (!pControl->Paint (hDC, rcPaint, pStopControl)) return false;
						}
					}
				}
			}
		}

		if (m_pVerticalScrollBar) {
			if (m_pVerticalScrollBar == pStopControl) return false;
			if (m_pVerticalScrollBar->IsVisible ()) {
				if (::IntersectRect (&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos ())) {
					if (!m_pVerticalScrollBar->Paint (hDC, rcPaint, pStopControl)) return false;
				}
			}
		}

		if (m_pHorizontalScrollBar) {
			if (m_pHorizontalScrollBar == pStopControl) return false;
			if (m_pHorizontalScrollBar->IsVisible ()) {
				if (::IntersectRect (&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos ())) {
					if (!m_pHorizontalScrollBar->Paint (hDC, rcPaint, pStopControl)) return false;
				}
			}
		}
		return true;
	}

	void CMenuElementUI::DrawItemIcon (HDC hDC, const RECT& rcItem) {
		if (!m_strIcon.empty () && !(m_bCheckItem && !GetChecked ())) {
			SIZE _cxyFixed = CMenuElementUI::m_cxyFixed;
			_cxyFixed.cx = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cx);
			_cxyFixed.cy = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cy);

			SIZE _szIconSize = CMenuElementUI::m_szIconSize;
			_szIconSize.cx = GetManager ()->GetDPIObj ()->Scale (_szIconSize.cx);
			_szIconSize.cy = GetManager ()->GetDPIObj ()->Scale (_szIconSize.cy);
			TListInfoUI* pInfo = m_pOwner->GetListInfo ();
			RECT rcTextPadding = pInfo->rcTextPadding;
			GetManager ()->GetDPIObj ()->Scale (&rcTextPadding);
			int padding = (rcTextPadding.left - _szIconSize.cx) / 2;
			RECT rcDest =
			{
				padding,
				(_cxyFixed.cy - _szIconSize.cy) / 2,
				padding + _szIconSize.cx,
				(_cxyFixed.cy - _szIconSize.cy) / 2 + _szIconSize.cy
			};
			GetManager ()->GetDPIObj ()->ScaleBack (&rcDest);
			faw::String pStrImage;
			pStrImage = faw::String::format (_T ("dest='%d,%d,%d,%d'"), rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);
			DrawImage (hDC, m_strIcon.str_view (), pStrImage.str_view ());
		}
	}

	void CMenuElementUI::DrawItemExpland (HDC hDC, const RECT& rcItem) {
		if (m_bShowExplandIcon) {
			faw::String strExplandIcon;
			strExplandIcon = GetManager ()->GetDefaultAttributeList (_T ("ExplandIcon"));
			if (strExplandIcon.empty ()) {
				return;
			}
			SIZE _cxyFixed = CMenuElementUI::m_cxyFixed;
			_cxyFixed.cx = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cx);
			_cxyFixed.cy = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cy);
			int padding = GetManager ()->GetDPIObj ()->Scale (ITEM_DEFAULT_EXPLAND_ICON_WIDTH) / 3;
			const TDrawInfo* pDrawInfo = GetManager ()->GetDrawInfo (strExplandIcon.str_view (), nullptr);
			const TImageInfo *pImageInfo = GetManager ()->GetImageEx (pDrawInfo->sImageName.str_view (), nullptr, 0);
			if (!pImageInfo) {
				return;
			}
			RECT rcDest =
			{
				_cxyFixed.cx - pImageInfo->nX - padding,
				(_cxyFixed.cy - pImageInfo->nY) / 2,
				_cxyFixed.cx - pImageInfo->nX - padding + pImageInfo->nX,
				(_cxyFixed.cy - pImageInfo->nY) / 2 + pImageInfo->nY
			};
			GetManager ()->GetDPIObj ()->ScaleBack (&rcDest);
			faw::String pStrImage;
			pStrImage = faw::String::format (_T ("dest='%d,%d,%d,%d'"), rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);
			DrawImage (hDC, strExplandIcon.str_view (), pStrImage.str_view ());
		}
	}


	void CMenuElementUI::DrawItemText (HDC hDC, const RECT& rcItem) {
		faw::String sText = GetText ();
		if (sText.empty ()) return;

		if (!m_pOwner) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo ();
		DWORD iTextColor = pInfo->dwTextColor;
		if ((m_uButtonState & UISTATE_HOT) != 0) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if (IsSelected ()) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if (!IsEnabled ()) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		int nLinks = 0;
		RECT rcText = rcItem;
		RECT rcTextPadding = pInfo->rcTextPadding;
		GetManager ()->GetDPIObj ()->Scale (&rcTextPadding);
		rcText.left += rcTextPadding.left;
		rcText.right -= rcTextPadding.right;
		rcText.top += rcTextPadding.top;
		rcText.bottom -= rcTextPadding.bottom;

		if (pInfo->bShowHtml)
			CRenderEngine::DrawHtmlText (hDC, m_pManager, rcText, sText.str_view (), iTextColor, \
				nullptr, nullptr, nLinks, pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
		else
			CRenderEngine::DrawText (hDC, m_pManager, rcText, sText.str_view (), iTextColor, \
				pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
	}


	SIZE CMenuElementUI::EstimateSize (SIZE szAvailable) {
		SIZE _cxyFixed = CMenuElementUI::m_cxyFixed;
		_cxyFixed.cx = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cx);
		_cxyFixed.cy = GetManager ()->GetDPIObj ()->Scale (_cxyFixed.cy);
		SIZE cXY = { 0 };
		for (int it = 0; it < GetCount (); it++) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt (it));
			if (!pControl->IsVisible ()) continue;
			SIZE sz = pControl->EstimateSize (szAvailable);
			cXY.cy += sz.cy;
			if (cXY.cx < sz.cx)
				cXY.cx = sz.cx;
		}
		if (cXY.cy == 0) {
			TListInfoUI* pInfo = m_pOwner->GetListInfo ();

			DWORD iTextColor = pInfo->dwTextColor;
			if ((m_uButtonState & UISTATE_HOT) != 0) {
				iTextColor = pInfo->dwHotTextColor;
			}
			if (IsSelected ()) {
				iTextColor = pInfo->dwSelectedTextColor;
			}
			if (!IsEnabled ()) {
				iTextColor = pInfo->dwDisabledTextColor;
			}
			faw::String sText = GetText ();

			RECT rcText = { 0, 0, MAX (szAvailable.cx, _cxyFixed.cx), 9999 };
			RECT rcTextPadding = pInfo->rcTextPadding;
			GetManager ()->GetDPIObj ()->Scale (&rcTextPadding);
			rcText.left += rcTextPadding.left;
			rcText.right -= rcTextPadding.right;
			if (pInfo->bShowHtml) {
				int nLinks = 0;
				CRenderEngine::DrawHtmlText (m_pManager->GetPaintDC (), m_pManager, rcText, sText.str_view (), iTextColor, nullptr, nullptr, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
			} else {
				CRenderEngine::DrawText (m_pManager->GetPaintDC (), m_pManager, rcText, sText.str_view (), iTextColor, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
			}
			cXY.cx = rcText.right - rcText.left + rcTextPadding.left + rcTextPadding.right;
			cXY.cy = rcText.bottom - rcText.top + rcTextPadding.top + rcTextPadding.bottom;
		}

		if (_cxyFixed.cy != 0) cXY.cy = _cxyFixed.cy;
		if (cXY.cx < _cxyFixed.cx)
			cXY.cx = _cxyFixed.cx;

		CMenuElementUI::m_cxyFixed.cy = MulDiv (cXY.cy, 100, GetManager ()->GetDPIObj ()->GetScale ());
		CMenuElementUI::m_cxyFixed.cx = MulDiv (cXY.cx, 100, GetManager ()->GetDPIObj ()->GetScale ());
		return cXY;
	}

	void CMenuElementUI::DoEvent (TEventUI& event) {
		if (event.Type == UIEVENT_MOUSEENTER) {
			CListContainerElementUI::DoEvent (event);
			if (m_pWindow) return;
			bool hasSubMenu = false;
			for (int i = 0; i < GetCount (); ++i) {
				if (GetItemAt (i)->GetInterface (_T ("MenuElement"))) {
					(static_cast<CMenuElementUI*>(GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetVisible (true);
					(static_cast<CMenuElementUI*>(GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetInternVisible (true);

					hasSubMenu = true;
				}
			}
			if (hasSubMenu) {
				m_pOwner->SelectItem (GetIndex (), true);
				CreateMenuWnd ();
			} else {
				ContextMenuParam param;
				param.hWnd = m_pManager->GetPaintWindow ();
				param.wParam = 2;
				CMenuWnd::GetGlobalContextMenuObserver ().RBroadcast (param);
				m_pOwner->SelectItem (GetIndex (), true);
			}
			return;
		}


		if (event.Type == UIEVENT_MOUSELEAVE) {

			bool hasSubMenu = false;
			for (int i = 0; i < GetCount (); ++i) {
				if (GetItemAt (i)->GetInterface (_T ("MenuElement"))) {

					hasSubMenu = true;
				}
			}

			if (!hasSubMenu) {
				m_pOwner->SelectItem (-1, true);
			}
		}

		if (event.Type == UIEVENT_BUTTONUP) {
			if (IsEnabled ()) {
				CListContainerElementUI::DoEvent (event);

				if (m_pWindow) return;

				bool hasSubMenu = false;
				for (int i = 0; i < GetCount (); ++i) {
					if (GetItemAt (i)->GetInterface (_T ("MenuElement"))) {
						(static_cast<CMenuElementUI*>(GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetVisible (true);
						(static_cast<CMenuElementUI*>(GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetInternVisible (true);

						hasSubMenu = true;
					}
				}
				if (hasSubMenu) {
					CreateMenuWnd ();
				} else {
					SetChecked (!GetChecked ());


					bool isClosing = false;
					CMenuUI* menuUI = static_cast<CMenuUI*>(GetManager ()->GetRoot ());
					isClosing = (menuUI->m_pWindow->isClosing);
					if (IsWindow (GetManager ()->GetPaintWindow ()) && !isClosing) {
						if (CMenuWnd::GetGlobalContextMenuObserver ().GetManager ()) {

							MenuCmd* pMenuCmd = new MenuCmd ();
							pMenuCmd->szName = GetName ();
							pMenuCmd->szUserData = GetUserData ();
							pMenuCmd->szText = GetText ();
							pMenuCmd->bChecked = GetChecked ();
							if (!PostMessage (CMenuWnd::GetGlobalContextMenuObserver ().GetManager ()->GetPaintWindow (), WM_MENUCLICK, (WPARAM) pMenuCmd, (LPARAM) this)) {
								delete pMenuCmd;
								pMenuCmd = nullptr;
							}
						}
					}
					ContextMenuParam param;
					param.hWnd = m_pManager->GetPaintWindow ();
					param.wParam = 1;
					CMenuWnd::GetGlobalContextMenuObserver ().RBroadcast (param);
				}
			}

			return;
		}

		if (event.Type == UIEVENT_KEYDOWN && event.chKey == VK_RIGHT) {
			if (m_pWindow) return;
			bool hasSubMenu = false;
			for (int i = 0; i < GetCount (); ++i) {
				if (GetItemAt (i)->GetInterface (_T ("MenuElement"))) {
					(static_cast<CMenuElementUI*>(GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetVisible (true);
					(static_cast<CMenuElementUI*>(GetItemAt (i)->GetInterface (_T ("MenuElement"))))->SetInternVisible (true);
					hasSubMenu = true;
				}
			}
			if (hasSubMenu) {
				m_pOwner->SelectItem (GetIndex (), true);
				CreateMenuWnd ();
			} else {
				ContextMenuParam param;
				param.hWnd = m_pManager->GetPaintWindow ();
				param.wParam = 2;
				CMenuWnd::GetGlobalContextMenuObserver ().RBroadcast (param);
				m_pOwner->SelectItem (GetIndex (), true);
			}

			return;
		}

		CListContainerElementUI::DoEvent (event);
	}

	CMenuWnd* CMenuElementUI::GetMenuWnd () {
		return m_pWindow;
	}

	void CMenuElementUI::CreateMenuWnd () {
		if (m_pWindow) return;

		m_pWindow = new CMenuWnd ();
		ASSERT (m_pWindow);

		ContextMenuParam param;
		param.hWnd = m_pManager->GetPaintWindow ();
		param.wParam = 2;
		CMenuWnd::GetGlobalContextMenuObserver ().RBroadcast (param);

		m_pWindow->Init (static_cast<CMenuElementUI*>(this), _T (""), POINT { 0, 0 }, nullptr);
	}

	void CMenuElementUI::SetLineType () {
		m_bDrawLine = true;
		if (m_cxyFixed.cy == 0 || m_cxyFixed.cy == ITEM_DEFAULT_HEIGHT)
			SetFixedHeight (DEFAULT_LINE_HEIGHT);

		SetMouseChildEnabled (false);
		SetMouseEnabled (false);
		SetEnabled (false);
	}

	void CMenuElementUI::SetLineColor (DWORD color) {
		m_dwLineColor = color;
	}

	DWORD CMenuElementUI::GetLineColor () const {
		return m_dwLineColor;
	}
	void CMenuElementUI::SetLinePadding (RECT rcInset) {
		m_rcLinePadding = rcInset;
	}

	RECT CMenuElementUI::GetLinePadding () const {
		return m_rcLinePadding;
	}

	void CMenuElementUI::SetIcon (faw::string_view_t strIcon) {
		if (!strIcon.empty ())
			m_strIcon = strIcon;
	}

	void CMenuElementUI::SetIconSize (LONG cx, LONG cy) {
		m_szIconSize.cx = cx;
		m_szIconSize.cy = cy;
	}

	void CMenuElementUI::SetChecked (bool bCheck/* = true*/) {
		SetItemInfo (GetName (), bCheck);
	}

	bool CMenuElementUI::GetChecked () const {
		faw::string_view_t pstrName = GetName ();
		if (pstrName.empty ()) return false;

		CStdStringPtrMap* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver ().GetMenuCheckInfo ();
		if (mCheckInfos) {
			MenuItemInfo* pItemInfo = (MenuItemInfo*) mCheckInfos->Find (pstrName);
			if (pItemInfo) {
				return pItemInfo->bChecked;
			}
		}
		return false;

	}

	void CMenuElementUI::SetCheckItem (bool bCheckItem/* = false*/) {
		m_bCheckItem = bCheckItem;
	}

	bool CMenuElementUI::GetCheckItem () const {
		return m_bCheckItem;
	}

	void CMenuElementUI::SetShowExplandIcon (bool bShow) {
		m_bShowExplandIcon = bShow;
	}

	void CMenuElementUI::SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue) {
		if (pstrName == _T ("icon")) {
			SetIcon (pstrValue);
		} else if (pstrName == _T ("iconsize")) {
			SIZE sz = FawTools::parse_size (pstrValue);
			SetIconSize (sz.cx, sz.cy);
		} else if (pstrName == _T ("checkitem")) {
			SetCheckItem (FawTools::parse_bool (pstrValue));
		} else if (pstrName == _T ("ischeck")) {
			CStdStringPtrMap* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver ().GetMenuCheckInfo ();
			if (mCheckInfos) {
				bool bFind = false;
				for (int i = 0; i < mCheckInfos->GetSize (); i++) {
					MenuItemInfo* itemInfo = (MenuItemInfo*) mCheckInfos->GetAt (i)->Data;
					if (GetName () == itemInfo->szName) {
						bFind = true;
						break;
					}
				}
				if (!bFind) SetChecked (FawTools::parse_bool (pstrValue));
			}
		} else if (pstrName == _T ("linetype")) {
			if (FawTools::parse_bool (pstrValue))
				SetLineType ();
		} else if (pstrName == _T ("expland")) {
			SetShowExplandIcon (FawTools::parse_bool (pstrValue));
		} else if (pstrName == _T ("linecolor")) {
			SetLineColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("linepadding")) {
			RECT rcInset = FawTools::parse_rect (pstrValue);
			SetLinePadding (rcInset);
		} else if (pstrName == _T ("height")) {
			SetFixedHeight (FawTools::parse_dec (pstrValue));
		} else
			CListContainerElementUI::SetAttribute (pstrName, pstrValue);
	}


	MenuItemInfo* CMenuElementUI::GetItemInfo (faw::string_view_t pstrName) {
		if (pstrName.empty ()) return nullptr;

		CStdStringPtrMap* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver ().GetMenuCheckInfo ();
		if (mCheckInfos) {
			MenuItemInfo* pItemInfo = (MenuItemInfo*) mCheckInfos->Find (pstrName);
			if (pItemInfo) {
				return pItemInfo;
			}
		}

		return nullptr;
	}

	MenuItemInfo* CMenuElementUI::SetItemInfo (faw::string_view_t pstrName, bool bChecked) {
		if (pstrName.empty ()) return nullptr;

		CStdStringPtrMap* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver ().GetMenuCheckInfo ();
		if (mCheckInfos) {
			MenuItemInfo* pItemInfo = (MenuItemInfo*) mCheckInfos->Find (pstrName);
			if (!pItemInfo) {
				pItemInfo = new MenuItemInfo;
				pItemInfo->szName = pstrName;
				pItemInfo->bChecked = bChecked;
				mCheckInfos->Insert (pstrName, pItemInfo);
			} else {
				pItemInfo->bChecked = bChecked;
			}

			return pItemInfo;
		}
		return nullptr;
	}
} // namespace DuiLib
