#include "StdAfx.h"
#include "UIScrollBar.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CScrollBarUI)

	CScrollBarUI::CScrollBarUI () {
		m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
	}

	faw::string_view_t CScrollBarUI::GetClass () const {
		return _T ("ScrollBarUI");
	}

	LPVOID CScrollBarUI::GetInterface (faw::string_view_t pstrName) {
		if (pstrName == DUI_CTRL_SCROLLBAR) return static_cast<CScrollBarUI*>(this);
		return CControlUI::GetInterface (pstrName);
	}

	CContainerUI* CScrollBarUI::GetOwner () const {
		return m_pOwner;
	}

	void CScrollBarUI::SetOwner (CContainerUI* pOwner) {
		m_pOwner = pOwner;
	}

	void CScrollBarUI::SetVisible (bool bVisible) {
		if (m_bVisible == bVisible) return;

		bool v = IsVisible ();
		m_bVisible = bVisible;
		if (m_bFocused) m_bFocused = false;

	}

	void CScrollBarUI::SetEnabled (bool bEnable) {
		CControlUI::SetEnabled (bEnable);
		if (!IsEnabled ()) {
			m_uButton1State = 0;
			m_uButton2State = 0;
			m_uThumbState = 0;
		}
	}

	void CScrollBarUI::SetFocus () {
		if (m_pOwner) m_pOwner->SetFocus ();
		else CControlUI::SetFocus ();
	}

	bool CScrollBarUI::IsHorizontal () {
		return m_bHorizontal;
	}

	void CScrollBarUI::SetHorizontal (bool bHorizontal) {
		if (m_bHorizontal == bHorizontal) return;

		m_bHorizontal = bHorizontal;
		if (m_bHorizontal) {
			if (m_cxyFixed.cy == 0) {
				m_cxyFixed.cx = 0;
				m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
			}
		} else {
			if (m_cxyFixed.cx == 0) {
				m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
				m_cxyFixed.cy = 0;
			}
		}

		if (m_pOwner) m_pOwner->NeedUpdate (); else NeedParentUpdate ();
	}

	int CScrollBarUI::GetScrollRange () const {
		return m_nRange;
	}

	void CScrollBarUI::SetScrollRange (int nRange) {
		if (m_nRange == nRange) return;

		m_nRange = nRange;
		if (m_nRange < 0) m_nRange = 0;
		if (m_nScrollPos > m_nRange) m_nScrollPos = m_nRange;
		SetPos (m_rcItem);
	}

	int CScrollBarUI::GetScrollPos () const {
		return m_nScrollPos;
	}

	void CScrollBarUI::SetScrollPos (int nPos) {
		if (m_nScrollPos == nPos) return;

		m_nScrollPos = nPos;
		if (m_nScrollPos < 0) m_nScrollPos = 0;
		if (m_nScrollPos > m_nRange) m_nScrollPos = m_nRange;
		SetPos (m_rcItem);
	}

	int CScrollBarUI::GetLineSize () const {
		return m_nLineSize;
	}

	void CScrollBarUI::SetLineSize (int nSize) {
		m_nLineSize = nSize;
	}

	bool CScrollBarUI::GetShowButton1 () {
		return m_bShowButton1;
	}

	void CScrollBarUI::SetShowButton1 (bool bShow) {
		m_bShowButton1 = bShow;
		SetPos (m_rcItem);
	}

	faw::string_view_t CScrollBarUI::GetButton1NormalImage () {
		return m_sButton1NormalImage.str_view ();
	}

	void CScrollBarUI::SetButton1NormalImage (faw::string_view_t pStrImage) {
		m_sButton1NormalImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetButton1HotImage () {
		return m_sButton1HotImage.str_view ();
	}

	void CScrollBarUI::SetButton1HotImage (faw::string_view_t pStrImage) {
		m_sButton1HotImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetButton1PushedImage () {
		return m_sButton1PushedImage.str_view ();
	}

	void CScrollBarUI::SetButton1PushedImage (faw::string_view_t pStrImage) {
		m_sButton1PushedImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetButton1DisabledImage () {
		return m_sButton1DisabledImage.str_view ();
	}

	void CScrollBarUI::SetButton1DisabledImage (faw::string_view_t pStrImage) {
		m_sButton1DisabledImage = pStrImage;
		Invalidate ();
	}

	bool CScrollBarUI::GetShowButton2 () {
		return m_bShowButton2;
	}

	void CScrollBarUI::SetShowButton2 (bool bShow) {
		m_bShowButton2 = bShow;
		SetPos (m_rcItem);
	}

	faw::string_view_t CScrollBarUI::GetButton2NormalImage () {
		return m_sButton2NormalImage.str_view ();
	}

	void CScrollBarUI::SetButton2NormalImage (faw::string_view_t pStrImage) {
		m_sButton2NormalImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetButton2HotImage () {
		return m_sButton2HotImage.str_view ();
	}

	void CScrollBarUI::SetButton2HotImage (faw::string_view_t pStrImage) {
		m_sButton2HotImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetButton2PushedImage () {
		return m_sButton2PushedImage.str_view ();
	}

	void CScrollBarUI::SetButton2PushedImage (faw::string_view_t pStrImage) {
		m_sButton2PushedImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetButton2DisabledImage () {
		return m_sButton2DisabledImage.str_view ();
	}

	void CScrollBarUI::SetButton2DisabledImage (faw::string_view_t pStrImage) {
		m_sButton2DisabledImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetThumbNormalImage () {
		return m_sThumbNormalImage.str_view ();
	}

	void CScrollBarUI::SetThumbNormalImage (faw::string_view_t pStrImage) {
		m_sThumbNormalImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetThumbHotImage () {
		return m_sThumbHotImage.str_view ();
	}

	void CScrollBarUI::SetThumbHotImage (faw::string_view_t pStrImage) {
		m_sThumbHotImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetThumbPushedImage () {
		return m_sThumbPushedImage.str_view ();
	}

	void CScrollBarUI::SetThumbPushedImage (faw::string_view_t pStrImage) {
		m_sThumbPushedImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetThumbDisabledImage () {
		return m_sThumbDisabledImage.str_view ();
	}

	void CScrollBarUI::SetThumbDisabledImage (faw::string_view_t pStrImage) {
		m_sThumbDisabledImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetRailNormalImage () {
		return m_sRailNormalImage.str_view ();
	}

	void CScrollBarUI::SetRailNormalImage (faw::string_view_t pStrImage) {
		m_sRailNormalImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetRailHotImage () {
		return m_sRailHotImage.str_view ();
	}

	void CScrollBarUI::SetRailHotImage (faw::string_view_t pStrImage) {
		m_sRailHotImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetRailPushedImage () {
		return m_sRailPushedImage.str_view ();
	}

	void CScrollBarUI::SetRailPushedImage (faw::string_view_t pStrImage) {
		m_sRailPushedImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetRailDisabledImage () {
		return m_sRailDisabledImage.str_view ();
	}

	void CScrollBarUI::SetRailDisabledImage (faw::string_view_t pStrImage) {
		m_sRailDisabledImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetBkNormalImage () {
		return m_sBkNormalImage.str_view ();
	}

	void CScrollBarUI::SetBkNormalImage (faw::string_view_t pStrImage) {
		m_sBkNormalImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetBkHotImage () {
		return m_sBkHotImage.str_view ();
	}

	void CScrollBarUI::SetBkHotImage (faw::string_view_t pStrImage) {
		m_sBkHotImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetBkPushedImage () {
		return m_sBkPushedImage.str_view ();
	}

	void CScrollBarUI::SetBkPushedImage (faw::string_view_t pStrImage) {
		m_sBkPushedImage = pStrImage;
		Invalidate ();
	}

	faw::string_view_t CScrollBarUI::GetBkDisabledImage () {
		return m_sBkDisabledImage.str_view ();
	}

	void CScrollBarUI::SetBkDisabledImage (faw::string_view_t pStrImage) {
		m_sBkDisabledImage = pStrImage;
		Invalidate ();
	}

	void CScrollBarUI::SetPos (RECT rc, bool bNeedInvalidate) {
		CControlUI::SetPos (rc, bNeedInvalidate);
		SIZE cxyFixed = m_cxyFixed;
		if (m_pManager) {
			GetManager ()->GetDPIObj ()->Scale (&cxyFixed);
		}
		rc = m_rcItem;
		if (m_bHorizontal) {
			int cx = rc.right - rc.left;
			if (m_bShowButton1) cx -= cxyFixed.cy;
			if (m_bShowButton2) cx -= cxyFixed.cy;
			if (cx > cxyFixed.cy) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if (m_bShowButton1) {
					m_rcButton1.right = rc.left + cxyFixed.cy;
					m_rcButton1.bottom = rc.top + cxyFixed.cy;
				} else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if (m_bShowButton2) {
					m_rcButton2.left = rc.right - cxyFixed.cy;
					m_rcButton2.bottom = rc.top + cxyFixed.cy;
				} else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				m_rcThumb.top = rc.top;
				m_rcThumb.bottom = rc.top + cxyFixed.cy;
				if (m_nRange > 0) {
					int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
					if (cxThumb < cxyFixed.cy) cxThumb = cxyFixed.cy;

					m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
					m_rcThumb.right = m_rcThumb.left + cxThumb;
					if (m_rcThumb.right > m_rcButton2.left) {
						m_rcThumb.left = m_rcButton2.left - cxThumb;
						m_rcThumb.right = m_rcButton2.left;
					}
				} else {
					m_rcThumb.left = m_rcButton1.right;
					m_rcThumb.right = m_rcButton2.left;
				}
			} else {
				int cxButton = (rc.right - rc.left) / 2;
				if (cxButton > cxyFixed.cy) cxButton = cxyFixed.cy;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if (m_bShowButton1) {
					m_rcButton1.right = rc.left + cxButton;
					m_rcButton1.bottom = rc.top + cxyFixed.cy;
				} else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if (m_bShowButton2) {
					m_rcButton2.left = rc.right - cxButton;
					m_rcButton2.bottom = rc.top + cxyFixed.cy;
				} else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				::ZeroMemory (&m_rcThumb, sizeof (m_rcThumb));
			}
		} else {
			int cy = rc.bottom - rc.top;
			if (m_bShowButton1) cy -= cxyFixed.cx;
			if (m_bShowButton2) cy -= cxyFixed.cx;
			if (cy > cxyFixed.cx) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if (m_bShowButton1) {
					m_rcButton1.right = rc.left + cxyFixed.cx;
					m_rcButton1.bottom = rc.top + cxyFixed.cx;
				} else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if (m_bShowButton2) {
					m_rcButton2.top = rc.bottom - cxyFixed.cx;
					m_rcButton2.right = rc.left + cxyFixed.cx;
				} else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				m_rcThumb.left = rc.left;
				m_rcThumb.right = rc.left + cxyFixed.cx;
				if (m_nRange > 0) {
					int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
					if (cyThumb < cxyFixed.cx) cyThumb = cxyFixed.cx;

					m_rcThumb.top = (m_nScrollPos * 1.0f / m_nRange) * (cy - cyThumb) + m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcThumb.top + cyThumb;
					if (m_rcThumb.bottom > m_rcButton2.top) {
						m_rcThumb.top = m_rcButton2.top - cyThumb;
						m_rcThumb.bottom = m_rcButton2.top;
					}
				} else {
					m_rcThumb.top = m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcButton2.top;
				}
			} else {
				int cyButton = (rc.bottom - rc.top) / 2;
				if (cyButton > cxyFixed.cx) cyButton = cxyFixed.cx;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if (m_bShowButton1) {
					m_rcButton1.right = rc.left + cxyFixed.cx;
					m_rcButton1.bottom = rc.top + cyButton;
				} else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if (m_bShowButton2) {
					m_rcButton2.top = rc.bottom - cyButton;
					m_rcButton2.right = rc.left + cxyFixed.cx;
				} else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				::ZeroMemory (&m_rcThumb, sizeof (m_rcThumb));
			}
		}
	}

	void CScrollBarUI::DoEvent (TEventUI& event) {
		if (!IsMouseEnabled () && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pOwner) m_pOwner->DoEvent (event);
			else CControlUI::DoEvent (event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS) {
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS) {
			return;
		}
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
			if (!IsEnabled ()) return;

			m_nLastScrollOffset = 0;
			m_nScrollRepeatDelay = 0;
			m_pManager->SetTimer (this, DEFAULT_TIMERID, 50U);

			if (::PtInRect (&m_rcButton1, event.ptMouse)) {
				m_uButton1State |= UISTATE_PUSHED;
				if (!m_bHorizontal) {
					if (m_pOwner) m_pOwner->LineUp ();
					else SetScrollPos (m_nScrollPos - m_nLineSize);
				} else {
					if (m_pOwner) m_pOwner->LineLeft ();
					else SetScrollPos (m_nScrollPos - m_nLineSize);
				}
			} else if (::PtInRect (&m_rcButton2, event.ptMouse)) {
				m_uButton2State |= UISTATE_PUSHED;
				if (!m_bHorizontal) {
					if (m_pOwner) m_pOwner->LineDown ();
					else SetScrollPos (m_nScrollPos + m_nLineSize);
				} else {
					if (m_pOwner) m_pOwner->LineRight ();
					else SetScrollPos (m_nScrollPos + m_nLineSize);
				}
			} else if (::PtInRect (&m_rcThumb, event.ptMouse)) {
				m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
				m_ptLastMouse = event.ptMouse;
				m_nLastScrollPos = m_nScrollPos;
			} else {
				if (!m_bHorizontal) {
					if (event.ptMouse.y < m_rcThumb.top) {
						if (m_pOwner) m_pOwner->PageUp ();
						else SetScrollPos (m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
					} else if (event.ptMouse.y > m_rcThumb.bottom) {
						if (m_pOwner) m_pOwner->PageDown ();
						else SetScrollPos (m_nScrollPos - m_rcItem.top + m_rcItem.bottom);
					}
				} else {
					if (event.ptMouse.x < m_rcThumb.left) {
						if (m_pOwner) m_pOwner->PageLeft ();
						else SetScrollPos (m_nScrollPos + m_rcItem.left - m_rcItem.right);
					} else if (event.ptMouse.x > m_rcThumb.right) {
						if (m_pOwner) m_pOwner->PageRight ();
						else SetScrollPos (m_nScrollPos - m_rcItem.left + m_rcItem.right);
					}
				}
			}
			if (m_pManager) m_pManager->SendNotify (this, DUI_MSGTYPE_SCROLL);
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP) {
			m_nScrollRepeatDelay = 0;
			m_nLastScrollOffset = 0;
			m_pManager->KillTimer (this, DEFAULT_TIMERID);

			if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
				m_uThumbState &= ~(UISTATE_CAPTURED | UISTATE_PUSHED);
				Invalidate ();
			} else if ((m_uButton1State & UISTATE_PUSHED) != 0) {
				m_uButton1State &= ~UISTATE_PUSHED;
				Invalidate ();
			} else if ((m_uButton2State & UISTATE_PUSHED) != 0) {
				m_uButton2State &= ~UISTATE_PUSHED;
				Invalidate ();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE) {
			if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
				int64_t fMouseRange = (event.ptMouse.y - m_ptLastMouse.y) * m_nRange;
				if (!m_bHorizontal) {
					int vRange = m_rcItem.bottom - m_rcItem.top - (m_rcThumb.bottom - m_rcThumb.top) - 2 * m_cxyFixed.cx;
					if (vRange != 0) m_nLastScrollOffset = fMouseRange / abs (vRange);
				} else {
					int hRange = m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left - 2 * m_cxyFixed.cy;
					if (hRange != 0) m_nLastScrollOffset = fMouseRange / abs (hRange);
				}
			} else {
				if ((m_uThumbState & UISTATE_HOT) != 0) {
					if (!::PtInRect (&m_rcThumb, event.ptMouse)) {
						m_uThumbState &= ~UISTATE_HOT;
						Invalidate ();
					}
				} else {
					if (!IsEnabled ()) return;
					if (::PtInRect (&m_rcThumb, event.ptMouse)) {
						m_uThumbState |= UISTATE_HOT;
						Invalidate ();
					}
				}
			}
			return;
		}
		if (event.Type == UIEVENT_CONTEXTMENU) {
			return;
		}
		if (event.Type == UIEVENT_TIMER && event.wParam == DEFAULT_TIMERID) {
			++m_nScrollRepeatDelay;
			if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
				if (!m_bHorizontal) {
					if (m_pOwner) m_pOwner->SetScrollPos ({ m_pOwner->GetScrollPos ().cx, m_nLastScrollPos + m_nLastScrollOffset });
					else SetScrollPos (m_nLastScrollPos + m_nLastScrollOffset);
				} else {
					if (m_pOwner) m_pOwner->SetScrollPos ({ m_nLastScrollPos + m_nLastScrollOffset, m_pOwner->GetScrollPos ().cy });
					else SetScrollPos (m_nLastScrollPos + m_nLastScrollOffset);
				}
				Invalidate ();
			} else if ((m_uButton1State & UISTATE_PUSHED) != 0) {
				if (m_nScrollRepeatDelay <= 5) return;
				if (!m_bHorizontal) {
					if (m_pOwner) m_pOwner->LineUp ();
					else SetScrollPos (m_nScrollPos - m_nLineSize);
				} else {
					if (m_pOwner) m_pOwner->LineLeft ();
					else SetScrollPos (m_nScrollPos - m_nLineSize);
				}
			} else if ((m_uButton2State & UISTATE_PUSHED) != 0) {
				if (m_nScrollRepeatDelay <= 5) return;
				if (!m_bHorizontal) {
					if (m_pOwner) m_pOwner->LineDown ();
					else SetScrollPos (m_nScrollPos + m_nLineSize);
				} else {
					if (m_pOwner) m_pOwner->LineRight ();
					else SetScrollPos (m_nScrollPos + m_nLineSize);
				}
			} else {
				if (m_nScrollRepeatDelay <= 5) return;
				POINT pt = { 0 };
				::GetCursorPos (&pt);
				::ScreenToClient (m_pManager->GetPaintWindow (), &pt);
				if (!m_bHorizontal) {
					if (pt.y < m_rcThumb.top) {
						if (m_pOwner) m_pOwner->PageUp ();
						else SetScrollPos (m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
					} else if (pt.y > m_rcThumb.bottom) {
						if (m_pOwner) m_pOwner->PageDown ();
						else SetScrollPos (m_nScrollPos - m_rcItem.top + m_rcItem.bottom);
					}
				} else {
					if (pt.x < m_rcThumb.left) {
						if (m_pOwner) m_pOwner->PageLeft ();
						else SetScrollPos (m_nScrollPos + m_rcItem.left - m_rcItem.right);
					} else if (pt.x > m_rcThumb.right) {
						if (m_pOwner) m_pOwner->PageRight ();
						else SetScrollPos (m_nScrollPos - m_rcItem.left + m_rcItem.right);
					}
				}
			}
			if (m_pManager) m_pManager->SendNotify (this, DUI_MSGTYPE_SCROLL);
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER) {
			if (IsEnabled ()) {
				m_uButton1State |= UISTATE_HOT;
				m_uButton2State |= UISTATE_HOT;
				if (::PtInRect (&m_rcThumb, event.ptMouse)) m_uThumbState |= UISTATE_HOT;
				Invalidate ();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE) {
			if (IsEnabled ()) {
				m_uButton1State &= ~UISTATE_HOT;
				m_uButton2State &= ~UISTATE_HOT;
				m_uThumbState &= ~UISTATE_HOT;
				Invalidate ();
			}
			return;
		}

		if (m_pOwner) m_pOwner->DoEvent (event); else CControlUI::DoEvent (event);
	}

	void CScrollBarUI::SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue) {
		if (pstrName == _T ("button1normalimage")) SetButton1NormalImage (pstrValue);
		else if (pstrName == _T ("button1hotimage")) SetButton1HotImage (pstrValue);
		else if (pstrName == _T ("button1pushedimage")) SetButton1PushedImage (pstrValue);
		else if (pstrName == _T ("button1disabledimage")) SetButton1DisabledImage (pstrValue);
		else if (pstrName == _T ("button2normalimage")) SetButton2NormalImage (pstrValue);
		else if (pstrName == _T ("button2hotimage")) SetButton2HotImage (pstrValue);
		else if (pstrName == _T ("button2pushedimage")) SetButton2PushedImage (pstrValue);
		else if (pstrName == _T ("button2disabledimage")) SetButton2DisabledImage (pstrValue);
		else if (pstrName == _T ("thumbnormalimage")) SetThumbNormalImage (pstrValue);
		else if (pstrName == _T ("thumbhotimage")) SetThumbHotImage (pstrValue);
		else if (pstrName == _T ("thumbpushedimage")) SetThumbPushedImage (pstrValue);
		else if (pstrName == _T ("thumbdisabledimage")) SetThumbDisabledImage (pstrValue);
		else if (pstrName == _T ("railnormalimage")) SetRailNormalImage (pstrValue);
		else if (pstrName == _T ("railhotimage")) SetRailHotImage (pstrValue);
		else if (pstrName == _T ("railpushedimage")) SetRailPushedImage (pstrValue);
		else if (pstrName == _T ("raildisabledimage")) SetRailDisabledImage (pstrValue);
		else if (pstrName == _T ("bknormalimage")) SetBkNormalImage (pstrValue);
		else if (pstrName == _T ("bkhotimage")) SetBkHotImage (pstrValue);
		else if (pstrName == _T ("bkpushedimage")) SetBkPushedImage (pstrValue);
		else if (pstrName == _T ("bkdisabledimage")) SetBkDisabledImage (pstrValue);
		else if (pstrName == _T ("hor")) SetHorizontal (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("linesize")) SetLineSize (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("range")) SetScrollRange (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("value")) SetScrollPos (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("showbutton1")) SetShowButton1 (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("showbutton2")) SetShowButton2 (FawTools::parse_bool (pstrValue));
		else CControlUI::SetAttribute (pstrName, pstrValue);
	}

	bool CScrollBarUI::DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
		PaintBkColor (hDC);
		PaintBkImage (hDC);
		PaintBk (hDC);
		PaintButton1 (hDC);
		PaintButton2 (hDC);
		PaintThumb (hDC);
		PaintRail (hDC);
		PaintBorder (hDC);
		return true;
	}

	void CScrollBarUI::PaintBk (HDC hDC) {
		if (!IsEnabled ()) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~UISTATE_DISABLED;

		if ((m_uThumbState & UISTATE_DISABLED) != 0) {
			if (!m_sBkDisabledImage.empty ()) {
				if (!DrawImage (hDC, m_sBkDisabledImage.str_view ())) {
				} else return;
			}
		} else if ((m_uThumbState & UISTATE_PUSHED) != 0) {
			if (!m_sBkPushedImage.empty ()) {
				if (!DrawImage (hDC, m_sBkPushedImage.str_view ())) {
				} else return;
			}
		} else if ((m_uThumbState & UISTATE_HOT) != 0) {
			if (!m_sBkHotImage.empty ()) {
				if (!DrawImage (hDC, m_sBkHotImage.str_view ())) {
				} else return;
			}
		}

		if (!m_sBkNormalImage.empty ()) {
			if (!DrawImage (hDC, m_sBkNormalImage.str_view ())) {
			} else return;
		}
	}

	void CScrollBarUI::PaintButton1 (HDC hDC) {
		if (!m_bShowButton1) return;

		if (!IsEnabled ()) m_uButton1State |= UISTATE_DISABLED;
		else m_uButton1State &= ~UISTATE_DISABLED;

		int d1 = MulDiv (m_rcButton1.left - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d2 = MulDiv (m_rcButton1.top - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d3 = MulDiv (m_rcButton1.right - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d4 = MulDiv (m_rcButton1.bottom - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
		m_sImageModify.clear ();
		m_sImageModify = faw::String::format (_T ("dest='%d,%d,%d,%d'"), d1, d2, d3, d4);

		if ((m_uButton1State & UISTATE_DISABLED) != 0) {
			if (!m_sButton1DisabledImage.empty ()) {
				if (!DrawImage (hDC, m_sButton1DisabledImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uButton1State & UISTATE_PUSHED) != 0) {
			if (!m_sButton1PushedImage.empty ()) {
				if (!DrawImage (hDC, m_sButton1PushedImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uButton1State & UISTATE_HOT) != 0) {
			if (!m_sButton1HotImage.empty ()) {
				if (!DrawImage (hDC, m_sButton1HotImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		}

		if (!m_sButton1NormalImage.empty ()) {
			if (!DrawImage (hDC, m_sButton1NormalImage.str_view (), m_sImageModify.str_view ())) {
			} else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect (hDC, m_rcButton1, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintButton2 (HDC hDC) {
		if (!m_bShowButton2) return;

		if (!IsEnabled ()) m_uButton2State |= UISTATE_DISABLED;
		else m_uButton2State &= ~UISTATE_DISABLED;
		int d1 = MulDiv (m_rcButton2.left - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d2 = MulDiv (m_rcButton2.top - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d3 = MulDiv (m_rcButton2.right - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d4 = MulDiv (m_rcButton2.bottom - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
		m_sImageModify.clear ();
		m_sImageModify = faw::String::format (_T ("dest='%d,%d,%d,%d'"), d1, d2, d3, d4);

		if ((m_uButton2State & UISTATE_DISABLED) != 0) {
			if (!m_sButton2DisabledImage.empty ()) {
				if (!DrawImage (hDC, m_sButton2DisabledImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uButton2State & UISTATE_PUSHED) != 0) {
			if (!m_sButton2PushedImage.empty ()) {
				if (!DrawImage (hDC, m_sButton2PushedImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uButton2State & UISTATE_HOT) != 0) {
			if (!m_sButton2HotImage.empty ()) {
				if (!DrawImage (hDC, m_sButton2HotImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		}

		if (!m_sButton2NormalImage.empty ()) {
			if (!DrawImage (hDC, m_sButton2NormalImage.str_view (), m_sImageModify.str_view ())) {
			} else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect (hDC, m_rcButton2, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintThumb (HDC hDC) {
		if (m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0) return;
		if (!IsEnabled ()) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~UISTATE_DISABLED;
		int d1 = MulDiv (m_rcThumb.left - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d2 = MulDiv (m_rcThumb.top - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d3 = MulDiv (m_rcThumb.right - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
		int d4 = MulDiv (m_rcThumb.bottom - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
		m_sImageModify.clear ();
		m_sImageModify = faw::String::format (_T ("dest='%d,%d,%d,%d'"), d1, d2, d3, d4);

		if ((m_uThumbState & UISTATE_DISABLED) != 0) {
			if (!m_sThumbDisabledImage.empty ()) {
				if (!DrawImage (hDC, m_sThumbDisabledImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uThumbState & UISTATE_PUSHED) != 0) {
			if (!m_sThumbPushedImage.empty ()) {
				if (!DrawImage (hDC, m_sThumbPushedImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uThumbState & UISTATE_HOT) != 0) {
			if (!m_sThumbHotImage.empty ()) {
				if (!DrawImage (hDC, m_sThumbHotImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		}

		if (!m_sThumbNormalImage.empty ()) {
			if (!DrawImage (hDC, m_sThumbNormalImage.str_view (), m_sImageModify.str_view ())) {
			} else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect (hDC, m_rcThumb, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintRail (HDC hDC) {
		if (m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0) return;
		if (!IsEnabled ()) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~UISTATE_DISABLED;

		m_sImageModify.clear ();
		if (!m_bHorizontal) {
			int d1 = MulDiv (m_rcThumb.left - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
			int d2 = MulDiv ((m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top - m_cxyFixed.cx / 2, 100, GetManager ()->GetDPIObj ()->GetScale ());
			int d3 = MulDiv (m_rcThumb.right - m_rcItem.left, 100, GetManager ()->GetDPIObj ()->GetScale ());
			int d4 = MulDiv ((m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top + m_cxyFixed.cx - m_cxyFixed.cx / 2, 100, GetManager ()->GetDPIObj ()->GetScale ());
			m_sImageModify = faw::String::format (_T ("dest='%d,%d,%d,%d'"), d1, d2, d3, d4);
		} else {
			int d1 = MulDiv ((m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left - m_cxyFixed.cy / 2, 100, GetManager ()->GetDPIObj ()->GetScale ());
			int d2 = MulDiv (m_rcThumb.top - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
			int d3 = MulDiv ((m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left + m_cxyFixed.cy - m_cxyFixed.cy / 2, 100, GetManager ()->GetDPIObj ()->GetScale ());
			int d4 = MulDiv (m_rcThumb.bottom - m_rcItem.top, 100, GetManager ()->GetDPIObj ()->GetScale ());
			m_sImageModify = faw::String::format (_T ("dest='%d,%d,%d,%d'"), d1, d2, d3, d4);
		}

		if ((m_uThumbState & UISTATE_DISABLED) != 0) {
			if (!m_sRailDisabledImage.empty ()) {
				if (!DrawImage (hDC, m_sRailDisabledImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uThumbState & UISTATE_PUSHED) != 0) {
			if (!m_sRailPushedImage.empty ()) {
				if (!DrawImage (hDC, m_sRailPushedImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		} else if ((m_uThumbState & UISTATE_HOT) != 0) {
			if (!m_sRailHotImage.empty ()) {
				if (!DrawImage (hDC, m_sRailHotImage.str_view (), m_sImageModify.str_view ())) {
				} else return;
			}
		}

		if (!m_sRailNormalImage.empty ()) {
			if (!DrawImage (hDC, m_sRailNormalImage.str_view (), m_sImageModify.str_view ())) {
			} else return;
		}
	}
}
