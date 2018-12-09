#include "StdAfx.h"
#include "UIRing.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CRingUI)

	CRingUI::CRingUI () {}

	CRingUI::~CRingUI () {
		if (m_pManager) m_pManager->KillTimer (this, RING_TIMERID);

		DeleteImage ();
	}

	string_view_t CRingUI::GetClass () const {
		return _T ("RingUI");
	}

	LPVOID CRingUI::GetInterface (string_view_t pstrName) {
		if (pstrName == _T ("Ring")) return static_cast<CRingUI*>(this);
		return CLabelUI::GetInterface (pstrName);
	}

	void CRingUI::SetAttribute (string_view_t pstrName, string_view_t pstrValue) {
		if (pstrName == _T ("bkimage")) SetBkImage (pstrValue);
		else CLabelUI::SetAttribute (pstrName, pstrValue);
	}

	void CRingUI::SetBkImage (string_view_t pStrImage) {
		if (m_sBkImage == pStrImage) return;
		m_sBkImage = pStrImage;
		DeleteImage ();
		Invalidate ();
	}

	void CRingUI::PaintBkImage (HDC hDC) {
		if (!m_pBkimage)
			InitImage ();
		if (!m_pBkimage)
			return;

		RECT rcItem = m_rcItem;
		int iWidth = rcItem.right - rcItem.left;
		int iHeight = rcItem.bottom - rcItem.top;
		Gdiplus::PointF centerPos ((Gdiplus::REAL) (rcItem.left + iWidth / 2), (Gdiplus::REAL) (rcItem.top + iHeight / 2));

		Gdiplus::Graphics graphics (hDC);
		graphics.TranslateTransform (centerPos.X, centerPos.Y);
		graphics.RotateTransform (m_fCurAngle);
		graphics.TranslateTransform (-centerPos.X, -centerPos.Y);//还原源点
		graphics.DrawImage (m_pBkimage, rcItem.left, rcItem.top, iWidth, iHeight);
	}

	void CRingUI::DoEvent (TEventUI& event) {
		if (event.Type == UIEVENT_TIMER && event.wParam == RING_TIMERID) {
			if (m_fCurAngle > 359) {
				m_fCurAngle = 0;
			}
			m_fCurAngle += 36.0;
			Invalidate ();
		} else {
			CLabelUI::DoEvent (event);
		}
	}

	void CRingUI::InitImage () {
		m_pBkimage = CRenderEngine::GdiplusLoadImage (GetBkImage ());
		if (nullptr == m_pBkimage) return;
		if (m_pManager) m_pManager->SetTimer (this, RING_TIMERID, 100);
	}

	void CRingUI::DeleteImage () {
		if (m_pBkimage) {
			delete m_pBkimage;
			m_pBkimage = nullptr;
		}
	}
}
