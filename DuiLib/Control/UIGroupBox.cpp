#include "StdAfx.h"
#include "UIGroupBox.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CGroupBoxUI)

	//////////////////////////////////////////////////////////////////////////
	//
	CGroupBoxUI::CGroupBoxUI (): m_uTextStyle (DT_SINGLELINE | DT_VCENTER | DT_CENTER) {
		SetInset ({ 20, 25, 20, 20 });
	}

	CGroupBoxUI::~CGroupBoxUI () {}

	string_view_t CGroupBoxUI::GetClass () const {
		return _T ("GroupBoxUI");
	}

	LPVOID CGroupBoxUI::GetInterface (string_view_t pstrName) {
		if (pstrName == _T ("GroupBox")) return static_cast<CGroupBoxUI*>(this);
		return CVerticalLayoutUI::GetInterface (pstrName);
	}
	void CGroupBoxUI::SetTextColor (DWORD dwTextColor) {
		m_dwTextColor = dwTextColor;
		Invalidate ();
	}

	DWORD CGroupBoxUI::GetTextColor () const {
		return m_dwTextColor;
	}
	void CGroupBoxUI::SetDisabledTextColor (DWORD dwTextColor) {
		m_dwDisabledTextColor = dwTextColor;
		Invalidate ();
	}

	DWORD CGroupBoxUI::GetDisabledTextColor () const {
		return m_dwDisabledTextColor;
	}
	void CGroupBoxUI::SetFont (int index) {
		m_iFont = index;
		Invalidate ();
	}

	int CGroupBoxUI::GetFont () const {
		return m_iFont;
	}
	void CGroupBoxUI::PaintText (HDC hDC) {
		CDuiString sText = GetText ();
		if (sText.empty ()) {
			return;
		}

		if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor ();
		if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor ();
		if (sText.empty ()) return;

		RECT rcText = m_rcItem;
		::InflateRect (&rcText, -5, -5);
		SIZE szAvailable = { rcText.right - rcText.left, rcText.bottom - rcText.top };
		SIZE sz = CalcrectSize (szAvailable);

		//计算文字区域
		rcText.left = rcText.left + 15;
		rcText.top = rcText.top - 5;
		rcText.right = rcText.left + sz.cx;
		rcText.bottom = rcText.top + sz.cy;

		DWORD dwTextColor = m_dwTextColor;
		if (!IsEnabled ()) dwTextColor = m_dwDisabledTextColor;
		CRenderEngine::DrawText (hDC, m_pManager, rcText, sText, dwTextColor, m_iFont, m_uTextStyle, GetAdjustColor (m_dwBackColor));
	}
	void CGroupBoxUI::PaintBorder (HDC hDC) {
		int nBorderSize;
		SIZE cxyBorderRound = { 0 };
		RECT rcBorderSize = { 0 };
		if (m_pManager) {
			nBorderSize = GetManager ()->GetDPIObj ()->Scale (m_nBorderSize);
			cxyBorderRound = GetManager ()->GetDPIObj ()->Scale (m_cxyBorderRound);
			rcBorderSize = GetManager ()->GetDPIObj ()->Scale (m_rcBorderSize);
		} else {
			nBorderSize = m_nBorderSize;
			cxyBorderRound = m_cxyBorderRound;
			rcBorderSize = m_rcBorderSize;
		}

		if (nBorderSize > 0) {
			//RECT rcItem = m_rcItem;
			//rcItem.Deflate (5, 5);

			//if (cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0) {//画圆角边框
			//	if (IsFocused () && m_dwFocusBorderColor != 0)
			//		CRenderEngine::DrawRoundRect (hDC, rcItem, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor (m_dwFocusBorderColor));
			//	else
			//		CRenderEngine::DrawRoundRect (hDC, rcItem, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor (m_dwBorderColor));
			//} else {
			//	if (IsFocused () && m_dwFocusBorderColor != 0)
			//		CRenderEngine::DrawRect (hDC, rcItem, nBorderSize, GetAdjustColor (m_dwFocusBorderColor));
			//	else
			//		CRenderEngine::DrawRect (hDC, rcItem, nBorderSize, GetAdjustColor (m_dwBorderColor));
			//}
			Gdiplus::Bitmap gb (m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, PixelFormat32bppARGB);
			Gdiplus::Graphics gg (&gb);
			RECT rcItem = m_rcItem;
			rcItem.right -= rcItem.left;
			rcItem.bottom -= rcItem.top;
			rcItem.left = rcItem.top = 0;
			::InflateRect (&rcItem, -5, -5);
			DWORD dwColor = GetAdjustColor ((IsFocused () && m_dwFocusBorderColor != 0) ? m_dwFocusBorderColor : m_dwBorderColor);
			HDC gghdc = gg.GetHDC ();
			if (cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0) {
				CRenderEngine::DrawRoundRect (gghdc, rcItem, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy, dwColor);
			} else {
				CRenderEngine::DrawRect (gghdc, rcItem, nBorderSize, dwColor);
			}
			gg.ReleaseHDC (gghdc);

			// 清除文字位置的边框
			SIZE szAvailable = { rcItem.right - rcItem.left, rcItem.bottom - rcItem.top };
			SIZE sz = CalcrectSize (szAvailable);
			rcItem.left = rcItem.left + 15;
			rcItem.top = rcItem.top - 5;
			rcItem.right = rcItem.left + sz.cx;
			rcItem.bottom = rcItem.top + sz.cy;
			Gdiplus::Color trans_c ((Gdiplus::ARGB) 0);
			for (int i = rcItem.left; i <= rcItem.right; ++i)
				for (int j = rcItem.top; j <= rcItem.bottom; ++j)
					gb.SetPixel (i, j, trans_c);
			Gdiplus::Graphics gx (hDC);
			gx.DrawImage (&gb, Gdiplus::Rect (m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top), 0, 0, gb.GetWidth (), gb.GetHeight (), Gdiplus::UnitPixel);
		}

		PaintText (hDC);
	}

	SIZE CGroupBoxUI::CalcrectSize (SIZE szAvailable) {
		SIZE cxyFixed = GetFixedXY ();
		RECT rcText = { 0, 0, MAX (szAvailable.cx, cxyFixed.cx), 20 };

		CDuiString sText = GetText ();

		CRenderEngine::DrawText (m_pManager->GetPaintDC (), m_pManager, rcText, sText, m_dwTextColor, m_iFont, DT_CALCRECT | m_uTextStyle);
		SIZE cXY = { rcText.right - rcText.left, rcText.bottom - rcText.top };
		return cXY;
	}
	void CGroupBoxUI::SetAttribute (string_view_t pstrName, string_view_t pstrValue) {
		if (pstrName == _T ("textcolor")) {
			SetTextColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("disabledtextcolor")) {
			SetDisabledTextColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("font")) {
			SetFont (FawTools::parse_dec (pstrValue));
		}

		CVerticalLayoutUI::SetAttribute (pstrName, pstrValue);
	}
}
