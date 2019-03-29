#include "StdAfx.h"
#include "UIButton.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CButtonUI)

	CButtonUI::CButtonUI () {
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	}

	faw::string_view_t CButtonUI::GetClass () const {
		return _T ("ButtonUI");
	}

	LPVOID CButtonUI::GetInterface (faw::string_view_t pstrName) {
		if (pstrName == DUI_CTRL_BUTTON) return static_cast<CButtonUI*>(this);
		return CLabelUI::GetInterface (pstrName.data ());
	}

	UINT CButtonUI::GetControlFlags () const {
		return (IsKeyboardEnabled () ? UIFLAG_TABSTOP : 0) | (IsEnabled () ? UIFLAG_SETCURSOR : 0);
	}

	void CButtonUI::DoEvent (TEventUI& event) {
		if (!IsMouseEnabled () && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent) m_pParent->DoEvent (event);
			else CLabelUI::DoEvent (event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS) {
			Invalidate ();
		} else if (event.Type == UIEVENT_KILLFOCUS) {
			Invalidate ();
		} else if (event.Type == UIEVENT_KEYDOWN) {
			if (IsKeyboardEnabled ()) {
				if (event.chKey == VK_SPACE || event.chKey == VK_RETURN) {
					Activate ();
					return;
				}
			}
		} else if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
			if (::PtInRect (&m_rcItem, event.ptMouse) && IsEnabled ()) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate ();
			}
			return;
		} else if (event.Type == UIEVENT_MOUSEMOVE) {
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				if (::PtInRect (&m_rcItem, event.ptMouse)) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate ();
			}
			return;
		} else if (event.Type == UIEVENT_BUTTONUP) {
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate ();
				if (::PtInRect (&m_rcItem, event.ptMouse)) Activate ();
			}
			return;
		} else if (event.Type == UIEVENT_CONTEXTMENU) {
			if (IsContextMenuUsed ()) {
				m_pManager->SendNotify (this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
			return;
		} else if (event.Type == UIEVENT_MOUSEENTER) {
			if (IsEnabled ()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate ();
			}
		} else if (event.Type == UIEVENT_MOUSELEAVE) {
			if (IsEnabled ()) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate ();
			}
		}
		CLabelUI::DoEvent (event);
	}

	bool CButtonUI::Activate () {
		if (!CControlUI::Activate ()) return false;
		if (m_pManager) {
			m_pManager->SendNotify (this, DUI_MSGTYPE_CLICK);
			BindTriggerTabSel ();
		}
		return true;
	}

	void CButtonUI::SetEnabled (bool bEnable) {
		CControlUI::SetEnabled (bEnable);
		if (!IsEnabled ()) {
			m_uButtonState = UISTATE_DISABLED;
		} else {
			m_uButtonState = 0;
		}
	}


	void CButtonUI::SetHotFont (int index) {
		m_iHotFont = index;
		Invalidate ();
	}

	int CButtonUI::GetHotFont () const {
		return m_iHotFont;
	}

	void CButtonUI::SetPushedFont (int index) {
		m_iPushedFont = index;
		Invalidate ();
	}

	int CButtonUI::GetPushedFont () const {
		return m_iPushedFont;
	}

	void CButtonUI::SetFocusedFont (int index) {
		m_iFocusedFont = index;
		Invalidate ();
	}

	int CButtonUI::GetFocusedFont () const {
		return m_iFocusedFont;
	}

	void CButtonUI::SetHotBkColor (DWORD dwColor) {
		m_dwHotBkColor = dwColor;
		Invalidate ();
	}

	DWORD CButtonUI::GetHotBkColor () const {
		return m_dwHotBkColor;
	}

	void CButtonUI::SetPushedBkColor (DWORD dwColor) {
		m_dwPushedBkColor = dwColor;
		Invalidate ();
	}

	DWORD CButtonUI::GetPushedBkColor () const {
		return m_dwPushedBkColor;
	}

	void CButtonUI::SetDisabledBkColor (DWORD dwColor) {
		m_dwDisabledBkColor = dwColor;
		Invalidate ();
	}

	DWORD CButtonUI::GetDisabledBkColor () const {
		return m_dwDisabledBkColor;
	}

	void CButtonUI::SetHotTextColor (DWORD dwColor) {
		m_dwHotTextColor = dwColor;
	}

	DWORD CButtonUI::GetHotTextColor () const {
		return m_dwHotTextColor;
	}

	void CButtonUI::SetPushedTextColor (DWORD dwColor) {
		m_dwPushedTextColor = dwColor;
	}

	DWORD CButtonUI::GetPushedTextColor () const {
		return m_dwPushedTextColor;
	}

	void CButtonUI::SetFocusedTextColor (DWORD dwColor) {
		m_dwFocusedTextColor = dwColor;
	}

	DWORD CButtonUI::GetFocusedTextColor () const {
		return m_dwFocusedTextColor;
	}

	faw::string_view_t CButtonUI::GetNormalImage () {
		return m_sNormalImage.str_view ();
	}

	void CButtonUI::SetNormalImage (faw::string_view_t pStrImage) {
		m_sNormalImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CButtonUI::GetHotImage () {
		return m_sHotImage.str_view ();
	}

	void CButtonUI::SetHotImage (faw::string_view_t pStrImage) {
		m_sHotImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CButtonUI::GetPushedImage () {
		return m_sPushedImage.str_view ();
	}

	void CButtonUI::SetPushedImage (faw::string_view_t pStrImage) {
		m_sPushedImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CButtonUI::GetFocusedImage () {
		return m_sFocusedImage.str_view ();
	}

	void CButtonUI::SetFocusedImage (faw::string_view_t pStrImage) {
		m_sFocusedImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CButtonUI::GetDisabledImage () {
		return m_sDisabledImage.str_view ();
	}

	void CButtonUI::SetDisabledImage (faw::string_view_t pStrImage) {
		m_sDisabledImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CButtonUI::GetHotForeImage () {
		return m_sHotForeImage.str_view ();
	}

	void CButtonUI::SetHotForeImage (faw::string_view_t pStrImage) {
		m_sHotForeImage = pStrImage;
		Invalidate ();
	}

	void CButtonUI::SetStateCount (int nCount) {
		m_nStateCount = nCount;
		Invalidate ();
	}

	int CButtonUI::GetStateCount () const {
		return m_nStateCount;
	}

	faw::string_view_t CButtonUI::GetStateImage () {
		return m_sStateImage.str_view ();
	}

	void CButtonUI::SetStateImage (faw::string_view_t pStrImage) {
		m_sNormalImage.clear ();
		m_sStateImage = pStrImage;
		Invalidate ();
	}

	void CButtonUI::BindTabIndex (int _BindTabIndex) {
		if (_BindTabIndex >= 0)
			m_iBindTabIndex = _BindTabIndex;
	}

	void CButtonUI::BindTabLayoutName (faw::string_view_t _TabLayoutName) {
		if (!_TabLayoutName.empty ())
			m_sBindTabLayoutName = _TabLayoutName;
	}

	void CButtonUI::BindTriggerTabSel (int _SetSelectIndex /*= -1*/) {
		faw::string_view_t pstrName = GetBindTabLayoutName ();
		if (pstrName.empty () || (GetBindTabLayoutIndex () < 0 && _SetSelectIndex < 0))
			return;

		CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(GetManager ()->FindControl (pstrName));
		if (!pTabLayout) return;
		pTabLayout->SelectItem (_SetSelectIndex >= 0 ? _SetSelectIndex : GetBindTabLayoutIndex ());
	}

	void CButtonUI::RemoveBindTabIndex () {
		m_iBindTabIndex = -1;
		m_sBindTabLayoutName.clear ();
	}

	int CButtonUI::GetBindTabLayoutIndex () {
		return m_iBindTabIndex;
	}

	faw::string_view_t CButtonUI::GetBindTabLayoutName () {
		return m_sBindTabLayoutName.str_view ();
	}

	void CButtonUI::SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue) {
		if (pstrName == _T ("normalimage")) SetNormalImage (pstrValue);
		else if (pstrName == _T ("hotimage")) SetHotImage (pstrValue);
		else if (pstrName == _T ("pushedimage")) SetPushedImage (pstrValue);
		else if (pstrName == _T ("focusedimage")) SetFocusedImage (pstrValue);
		else if (pstrName == _T ("disabledimage")) SetDisabledImage (pstrValue);
		else if (pstrName == _T ("hotforeimage")) SetHotForeImage (pstrValue);
		else if (pstrName == _T ("stateimage")) SetStateImage (pstrValue);
		else if (pstrName == _T ("statecount")) SetStateCount (_ttoi (pstrValue.data ()));
		else if (pstrName == _T ("bindtabindex")) BindTabIndex (_ttoi (pstrValue.data ()));
		else if (pstrName == _T ("bindtablayoutname")) BindTabLayoutName (pstrValue);
		else if (pstrName == _T ("hotbkcolor")) {
			SetHotBkColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("pushedbkcolor")) {
			SetPushedBkColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("disabledbkcolor")) {
			SetDisabledBkColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("hottextcolor")) {
			SetHotTextColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("pushedtextcolor")) {
			SetPushedTextColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("focusedtextcolor")) {
			SetFocusedTextColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("hotfont")) SetHotFont (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("pushedfont")) SetPushedFont (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("focuedfont")) SetFocusedFont (FawTools::parse_dec (pstrValue));

		else CLabelUI::SetAttribute (pstrName, pstrValue);
	}

	void CButtonUI::PaintText (HDC hDC) {
		if (IsFocused ()) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~UISTATE_FOCUSED;
		if (!IsEnabled ()) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~UISTATE_DISABLED;

		if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor ();
		if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor ();

		faw::String sText = GetText ();
		if (sText.empty ()) return;

		RECT _rcTextPadding = CButtonUI::m_rcTextPadding;
		GetManager ()->GetDPIObj ()->Scale (&_rcTextPadding);
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += _rcTextPadding.left;
		rc.right -= _rcTextPadding.right;
		rc.top += _rcTextPadding.top;
		rc.bottom -= _rcTextPadding.bottom;

		DWORD clrColor = IsEnabled () ? m_dwTextColor : m_dwDisabledTextColor;

		if (((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor () != 0))
			clrColor = GetPushedTextColor ();
		else if (((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor () != 0))
			clrColor = GetHotTextColor ();
		else if (((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor () != 0))
			clrColor = GetFocusedTextColor ();

		int iFont = GetFont ();
		if (((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedFont () != -1))
			iFont = GetPushedFont ();
		else if (((m_uButtonState & UISTATE_HOT) != 0) && (GetHotFont () != -1))
			iFont = GetHotFont ();
		else if (((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedFont () != -1))
			iFont = GetFocusedFont ();

		if (m_bShowHtml)
			CRenderEngine::DrawHtmlText (hDC, m_pManager, rc, sText.str_view (), clrColor, nullptr, nullptr, nLinks, iFont, m_uTextStyle);
		else
			CRenderEngine::DrawText (hDC, m_pManager, rc, sText.str_view (), clrColor, iFont, m_uTextStyle);
	}

	void CButtonUI::PaintBkColor (HDC hDC) {
		if ((m_uButtonState & UISTATE_DISABLED) != 0) {
			if (m_dwDisabledBkColor != 0) {
				CRenderEngine::DrawColor (hDC, m_rcPaint, GetAdjustColor (m_dwDisabledBkColor));
				return;
			}
		} else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
			if (m_dwPushedBkColor != 0) {
				CRenderEngine::DrawColor (hDC, m_rcPaint, GetAdjustColor (m_dwPushedBkColor));
				return;
			}
		} else if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (m_dwHotBkColor != 0) {
				CRenderEngine::DrawColor (hDC, m_rcPaint, GetAdjustColor (m_dwHotBkColor));
				return;
			}
		}

		return CControlUI::PaintBkColor (hDC);
	}

	void CButtonUI::PaintStatusImage (HDC hDC) {
		if (!m_sStateImage.empty () && m_nStateCount > 0) {
			TDrawInfo info;
			info.Parse (m_sStateImage.str_view (), _T (""), m_pManager);
			const TImageInfo* pImage = m_pManager->GetImageEx (info.sImageName.str_view (), info.sResType.str_view (), info.dwMask, info.bHSL);
			if (m_sNormalImage.empty () && pImage) {
				SIZE szImage = { pImage->nX, pImage->nY };
				SIZE szStatus = { pImage->nX / m_nStateCount, pImage->nY };
				if (szImage.cx > 0 && szImage.cy > 0) {
					RECT rcSrc = { 0, 0, szImage.cx, szImage.cy };
					if (m_nStateCount > 0) {
						int iLeft = rcSrc.left + 0 * szStatus.cx;
						int iRight = iLeft + szStatus.cx;
						int iTop = rcSrc.top;
						int iBottom = iTop + szStatus.cy;
						m_sNormalImage = faw::String::format (_T ("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.c_str (), info.sResType.c_str (), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
					}
					if (m_nStateCount > 1) {
						int iLeft = rcSrc.left + 1 * szStatus.cx;
						int iRight = iLeft + szStatus.cx;
						int iTop = rcSrc.top;
						int iBottom = iTop + szStatus.cy;
						m_sHotImage = faw::String::format (_T ("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.c_str (), info.sResType.c_str (), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
						m_sPushedImage = faw::String::format (_T ("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.c_str (), info.sResType.c_str (), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
					}
					if (m_nStateCount > 2) {
						int iLeft = rcSrc.left + 2 * szStatus.cx;
						int iRight = iLeft + szStatus.cx;
						int iTop = rcSrc.top;
						int iBottom = iTop + szStatus.cy;
						m_sPushedImage = faw::String::format (_T ("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.c_str (), info.sResType.c_str (), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
					}
					if (m_nStateCount > 3) {
						int iLeft = rcSrc.left + 3 * szStatus.cx;
						int iRight = iLeft + szStatus.cx;
						int iTop = rcSrc.top;
						int iBottom = iTop + szStatus.cy;
						m_sDisabledImage = faw::String::format (_T ("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.c_str (), info.sResType.c_str (), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
					}
				}
			}
		}

		if (IsFocused ()) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~UISTATE_FOCUSED;
		if (!IsEnabled ()) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~UISTATE_DISABLED;
		if (!::IsWindowEnabled (m_pManager->GetPaintWindow ())) {
			m_uButtonState &= UISTATE_DISABLED;
		}
		if ((m_uButtonState & UISTATE_DISABLED) != 0) {
			if (!m_sDisabledImage.empty ()) {
				if (!DrawImage (hDC, m_sDisabledImage.str_view ())) {
				} else return;
			}
		} else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
			if (!m_sPushedImage.empty ()) {
				if (!DrawImage (hDC, m_sPushedImage.str_view ())) {
				} else return;
			}
		} else if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (!m_sHotImage.empty ()) {
				if (!DrawImage (hDC, m_sHotImage.str_view ())) {
				} else return;
			}
		} else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
			if (!m_sFocusedImage.empty ()) {
				if (!DrawImage (hDC, m_sFocusedImage.str_view ())) {
				} else return;
			}
		}

		if (!m_sNormalImage.empty ()) {
			if (!DrawImage (hDC, m_sNormalImage.str_view ())) {
			}
		}
	}

	void CButtonUI::PaintForeImage (HDC hDC) {
		if ((m_uButtonState & UISTATE_PUSHED) != 0) {
			if (!m_sPushedForeImage.empty ()) {
				if (!DrawImage (hDC, m_sPushedForeImage.str_view ())) {
				} else return;
			}
		} else if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (!m_sHotForeImage.empty ()) {
				if (!DrawImage (hDC, m_sHotForeImage.str_view ())) {
				} else return;
			}
		}
		if (!m_sForeImage.empty ()) {
			if (!DrawImage (hDC, m_sForeImage.str_view ())) {
			}
		}
	}
}