#include "StdAfx.h"
#include "UIFadeButton.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CFadeButtonUI)

	CFadeButtonUI::CFadeButtonUI (): CUIAnimation (&(*this)) {}

	CFadeButtonUI::~CFadeButtonUI () {
		StopAnimation ();
	}

	faw::string_view_t CFadeButtonUI::GetClass () const {
		return _T ("FadeButtonUI");
	}

	LPVOID CFadeButtonUI::GetInterface (faw::string_view_t pstrName) {
		if (pstrName == _T ("FadeButton"))
			return static_cast<CFadeButtonUI*>(this);
		return CButtonUI::GetInterface (pstrName);
	}

	void CFadeButtonUI::SetNormalImage (faw::string_view_t pStrImage) {
		m_sNormalImage = pStrImage;
		m_sLastImage = m_sNormalImage;
	}

	void CFadeButtonUI::DoEvent (TEventUI& event) {
		if (event.Type == UIEVENT_MOUSEENTER && !IsAnimationRunning (FADE_IN_ID)) {
			m_bFadeAlpha = 0;
			m_bMouseHove = TRUE;
			StopAnimation (FADE_OUT_ID);
			StartAnimation (FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_IN_ID);
			Invalidate ();
			return;
		} else if (event.Type == UIEVENT_MOUSELEAVE && !IsAnimationRunning (FADE_OUT_ID)) {
			m_bFadeAlpha = 0;
			m_bMouseLeave = TRUE;
			StopAnimation (FADE_IN_ID);
			StartAnimation (FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_OUT_ID);
			Invalidate ();
			return;
		} else if (event.Type == UIEVENT_TIMER) {
			OnTimer ((int) event.wParam);
		}
		CButtonUI::DoEvent (event);
	}

	void CFadeButtonUI::OnTimer (int nTimerID) {
		OnAnimationElapse (nTimerID);
	}

	void CFadeButtonUI::PaintStatusImage (HDC hDC) {
		if (IsFocused ()) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~UISTATE_FOCUSED;
		if (!IsEnabled ()) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~UISTATE_DISABLED;

		if ((m_uButtonState & UISTATE_DISABLED) != 0) {
			if (!m_sDisabledImage.empty ()) {
				if (DrawImage (hDC, m_sDisabledImage.str_view ()))
					return;
			}
		} else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
			if (!m_sPushedImage.empty ()) {
				if (DrawImage (hDC, m_sPushedImage.str_view ()))
					return;
			}
		} else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
			if (!m_sFocusedImage.empty ()) {
				if (DrawImage (hDC, m_sFocusedImage.str_view ()))
					return;
			}
		}

		if (!m_sNormalImage.empty ()) {
			if (IsAnimationRunning (FADE_IN_ID) || IsAnimationRunning (FADE_OUT_ID)) {
				if (m_bMouseHove) {
					m_bMouseHove = FALSE;
					m_sLastImage = m_sHotImage;
					DrawImage (hDC, m_sNormalImage.str_view ());
					return;
				} else if (m_bMouseLeave) {
					m_bMouseLeave = FALSE;
					m_sLastImage = m_sNormalImage;
					DrawImage (hDC, m_sHotImage.str_view ());
					return;
				}

				m_sOldImage = m_sNormalImage;
				m_sNewImage = m_sHotImage;
				if (IsAnimationRunning (FADE_OUT_ID)) {
					m_sOldImage = m_sHotImage;
					m_sNewImage = m_sNormalImage;
				}
				faw::String sFadeOut, sFadeIn;
				sFadeOut = faw::String::format (_T ("fade='%d'"), 255 - m_bFadeAlpha);
				sFadeIn = faw::String::format (_T ("fade='%d'"), m_bFadeAlpha);
				DrawImage (hDC, m_sOldImage.str_view (), sFadeOut.str_view ());
				DrawImage (hDC, m_sNewImage.str_view (), sFadeIn.str_view ());
				return;
			} else {
				if (m_bMouseHove) {
					m_bMouseHove = FALSE;
					m_sLastImage = m_sHotImage;
					DrawImage (hDC, m_sNormalImage.str_view ());
				} else if (m_bMouseLeave) {
					m_bMouseLeave = FALSE;
					m_sLastImage = m_sNormalImage;
					DrawImage (hDC, m_sHotImage.str_view ());
				} else {
					if (m_sLastImage.empty ())
						m_sLastImage = m_sNormalImage;
					DrawImage (hDC, m_sLastImage.str_view ());
				}
			}
		}
	}

	void CFadeButtonUI::OnAnimationStep (INT nTotalFrame, INT nCurFrame, INT nAnimationID) {
		m_bFadeAlpha = (BYTE) ((nCurFrame / (double) nTotalFrame) * 255);
		m_bFadeAlpha = m_bFadeAlpha == 0 ? 10 : m_bFadeAlpha;
		Invalidate ();
	}

} // namespace DuiLib