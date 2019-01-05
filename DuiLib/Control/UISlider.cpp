#include "StdAfx.h"
#include "UISlider.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CSliderUI)
	CSliderUI::CSliderUI () {
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_szThumb.cx = m_szThumb.cy = 10;
	}

	string_view_t CSliderUI::GetClass () const {
		return _T ("SliderUI");
	}

	UINT CSliderUI::GetControlFlags () const {
		if (IsEnabled ()) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID CSliderUI::GetInterface (string_view_t pstrName) {
		if (pstrName == DUI_CTRL_SLIDER) return static_cast<CSliderUI*>(this);
		return CProgressUI::GetInterface (pstrName);
	}

	void CSliderUI::SetEnabled (bool bEnable) {
		CControlUI::SetEnabled (bEnable);
		if (!IsEnabled ()) {
			m_uButtonState = 0;
		}
	}

	int CSliderUI::GetChangeStep () {
		return m_nStep;
	}

	void CSliderUI::SetChangeStep (int step) {
		m_nStep = step;
	}

	void CSliderUI::SetThumbSize (SIZE szXY) {
		m_szThumb = szXY;
	}

	RECT CSliderUI::GetThumbRect () const {
		RECT rcThumb = { 0 };
		SIZE _szThumb = CSliderUI::m_szThumb;
		if (GetManager ()) {
			GetManager ()->GetDPIObj ()->Scale (&_szThumb);
		}
		if (m_bHorizontal) {
			int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - _szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - _szThumb.cy) / 2;
			rcThumb = { left, top, left + _szThumb.cx, top + _szThumb.cy };
		} else {
			int left = (m_rcItem.right + m_rcItem.left - _szThumb.cx) / 2;
			int top = m_rcItem.bottom - _szThumb.cy - (m_rcItem.bottom - m_rcItem.top - _szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			rcThumb = { left, top, left + _szThumb.cx, top + _szThumb.cy };
		}
		if (m_pManager) {
			//m_pManager->GetDPIObj()->Scale(&rcThumb);
		}
		return rcThumb;
	}

	string_view_t CSliderUI::GetThumbImage () const {
		return m_sThumbImage;
	}

	void CSliderUI::SetThumbImage (string_view_t pStrImage) {
		m_sThumbImage = pStrImage;
		Invalidate ();
	}

	string_view_t CSliderUI::GetThumbHotImage () const {
		return m_sThumbHotImage;
	}

	void CSliderUI::SetThumbHotImage (string_view_t pStrImage) {
		m_sThumbHotImage = pStrImage;
		Invalidate ();
	}

	string_view_t CSliderUI::GetThumbPushedImage () const {
		return m_sThumbPushedImage;
	}

	void CSliderUI::SetThumbPushedImage (string_view_t pStrImage) {
		m_sThumbPushedImage = pStrImage;
		Invalidate ();
	}

	void CSliderUI::SetValue (int nValue) {
		if ((m_uButtonState & UISTATE_CAPTURED) != 0) return;
		CProgressUI::SetValue (nValue);
	}

	void CSliderUI::DoEvent (TEventUI& event) {
		if (!IsMouseEnabled () && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent) m_pParent->DoEvent (event);
			else CProgressUI::DoEvent (event);
			return;
		}

		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
			if (IsEnabled ()) {
				m_uButtonState |= UISTATE_CAPTURED;

				int nValue;
				if (m_bHorizontal) {
					if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) nValue = m_nMax;
					else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) nValue = m_nMin;
					else nValue = m_nMin + 1.0f * (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				} else {
					if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) nValue = m_nMin;
					else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) nValue = m_nMax;
					else nValue = m_nMin + 1.0f * (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if (m_nValue != nValue && nValue >= m_nMin && nValue <= m_nMax) {
					m_nValue = nValue;
					Invalidate ();
				}
				UpdateText ();
			}
			return;
		}

		if (event.Type == UIEVENT_BUTTONUP || event.Type == UIEVENT_RBUTTONUP) {
			if (IsEnabled ()) {
				int nValue = 0;
				if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
					m_uButtonState &= ~UISTATE_CAPTURED;
				}
				if (m_bHorizontal) {
					if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) nValue = m_nMax;
					else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) nValue = m_nMin;
					else nValue = m_nMin + 1.0f * (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				} else {
					if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) nValue = m_nMin;
					else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) nValue = m_nMax;
					else nValue = m_nMin + 1.0f * (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if (nValue >= m_nMin && nValue <= m_nMax) {
					m_nValue = nValue;
					m_pManager->SendNotify (this, DUI_MSGTYPE_VALUECHANGED);
					Invalidate ();
				}
				UpdateText ();
				return;
			}
		}
		if (event.Type == UIEVENT_CONTEXTMENU) {
			return;
		}
		if (event.Type == UIEVENT_SCROLLWHEEL) {
			if (IsEnabled ()) {
				switch (LOWORD (event.wParam)) {
				case SB_LINEUP:
					SetValue (GetValue () + GetChangeStep ());
					m_pManager->SendNotify (this, DUI_MSGTYPE_VALUECHANGED);
					return;
				case SB_LINEDOWN:
					SetValue (GetValue () - GetChangeStep ());
					m_pManager->SendNotify (this, DUI_MSGTYPE_VALUECHANGED);
					return;
				}
			}
		}
		if (event.Type == UIEVENT_MOUSEMOVE) {
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				if (m_bHorizontal) {
					if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) m_nValue = m_nMax;
					else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) m_nValue = m_nMin;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				} else {
					if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) m_nValue = m_nMin;
					else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) m_nValue = m_nMax;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if (m_bSendMove) {
					UpdateText ();
					m_pManager->SendNotify (this, DUI_MSGTYPE_VALUECHANGED_MOVE);
				}
				Invalidate ();
			}

			POINT pt = event.ptMouse;
			RECT rcThumb = GetThumbRect ();
			if (IsEnabled () && ::PtInRect (&rcThumb, event.ptMouse)) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate ();
			} else {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate ();
			}
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR) {
			RECT rcThumb = GetThumbRect ();
			if (IsEnabled ()) {
				::SetCursor (::LoadCursor (nullptr, IDC_HAND));
				return;
			}
		}
		if (event.Type == UIEVENT_MOUSELEAVE) {
			if (IsEnabled ()) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate ();
			}
			return;
		}
		CControlUI::DoEvent (event);
	}

	void CSliderUI::SetCanSendMove (bool bCanSend) {
		m_bSendMove = bCanSend;
	}
	bool CSliderUI::GetCanSendMove () const {
		return m_bSendMove;
	}

	void CSliderUI::SetAttribute (string_view_t pstrName, string_view_t pstrValue) {
		if (pstrName == _T ("thumbimage")) SetThumbImage (pstrValue);
		else if (pstrName == _T ("thumbhotimage")) SetThumbHotImage (pstrValue);
		else if (pstrName == _T ("thumbpushedimage")) SetThumbPushedImage (pstrValue);
		else if (pstrName == _T ("thumbsize")) {
			SIZE szXY = FawTools::parse_size (pstrValue);
			SetThumbSize (szXY);
		} else if (pstrName == _T ("step")) {
			SetChangeStep (FawTools::parse_dec (pstrValue));
		} else if (pstrName == _T ("sendmove")) {
			SetCanSendMove (FawTools::parse_bool (pstrValue));
		} else CProgressUI::SetAttribute (pstrName, pstrValue);
	}

	void CSliderUI::PaintForeImage (HDC hDC) {
		CProgressUI::PaintForeImage (hDC);

		RECT rcThumb = GetThumbRect ();
		rcThumb.left -= m_rcItem.left;
		rcThumb.top -= m_rcItem.top;
		rcThumb.right -= m_rcItem.left;
		rcThumb.bottom -= m_rcItem.top;

		GetManager ()->GetDPIObj ()->ScaleBack (&rcThumb);

		if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
			if (!m_sThumbPushedImage.empty ()) {
				m_sImageModify.clear ();
				m_sImageModify.Format (_T ("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if (!DrawImage (hDC, m_sThumbPushedImage, m_sImageModify)) {
				} else return;
			}
		} else if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (!m_sThumbHotImage.empty ()) {
				m_sImageModify.clear ();
				m_sImageModify.Format (_T ("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if (!DrawImage (hDC, m_sThumbHotImage, m_sImageModify)) {
				} else return;
			}
		}

		if (!m_sThumbImage.empty ()) {
			m_sImageModify.clear ();
			m_sImageModify.Format (_T ("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if (!DrawImage (hDC, m_sThumbImage, m_sImageModify)) {
			} else return;
		}
	}
}
