#include "StdAfx.h"
#include "UIComboBox.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CComboBoxUI)

	CComboBoxUI::CComboBoxUI () {}

	string_view_t CComboBoxUI::GetClass () const {
		return _T ("ComboBoxUI");
	}

	void CComboBoxUI::SetAttribute (string_view_t pstrName, string_view_t pstrValue) {
		if (pstrName == _T ("arrowimage"))
			m_sArrowImage = pstrValue;
		else
			CComboUI::SetAttribute (pstrName, pstrValue);
	}

	void CComboBoxUI::PaintStatusImage (HDC hDC) {
		if (m_sArrowImage.empty ())
			CComboUI::PaintStatusImage (hDC);
		else {
			// get index
			if (IsFocused ()) m_uButtonState |= UISTATE_FOCUSED;
			else m_uButtonState &= ~UISTATE_FOCUSED;
			if (!IsEnabled ()) m_uButtonState |= UISTATE_DISABLED;
			else m_uButtonState &= ~UISTATE_DISABLED;

			int nIndex = 0;
			if ((m_uButtonState & UISTATE_DISABLED) != 0)
				nIndex = 4;
			else if ((m_uButtonState & UISTATE_PUSHED) != 0)
				nIndex = 2;
			else if ((m_uButtonState & UISTATE_HOT) != 0)
				nIndex = 1;
			else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
				nIndex = 3;

			// make modify string
			CDuiString sModify = m_sArrowImage;

			size_t nPos1 = sModify.find (_T ("source"));
			size_t nPos2 = sModify.find (_T ("'"), nPos1 + 7);
			if (nPos2 == string_t::npos) return; //first
			size_t nPos3 = sModify.find (_T ("'"), nPos2 + 1);
			if (nPos3 == string_t::npos) return; //second

			RECT rcBmpPart = FawTools::parse_rect (string_view_t (&sModify[nPos2 + 1]));

			m_nArrowWidth = (rcBmpPart.right - rcBmpPart.left) / 5;
			rcBmpPart.left += nIndex * m_nArrowWidth;
			rcBmpPart.right = rcBmpPart.left + m_nArrowWidth;

			RECT rcDest { 0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top };
			::InflateRect (&rcDest, -GetBorderSize (), -GetBorderSize ());
			rcDest.left = rcDest.right - m_nArrowWidth;

			CDuiString sSource = sModify.Mid (nPos1, nPos3 + 1 - nPos1);
			CDuiString sReplace;
			sReplace.Format (_T ("source='%d,%d,%d,%d' dest='%d,%d,%d,%d'"),
				rcBmpPart.left, rcBmpPart.top, rcBmpPart.right, rcBmpPart.bottom,
				rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);

			sModify.Replace (sSource, sReplace);

			// draw image
			if (!DrawImage (hDC, m_sArrowImage, sModify)) {
			}
		}
	}

	void CComboBoxUI::PaintText (HDC hDC) {
		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;

		rcText.right -= m_nArrowWidth; // add this line than CComboUI::PaintText(HDC hDC)

		if (m_iCurSel >= 0) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface (_T ("ListItem")));
			if (pElement) {
				pElement->DrawItemText (hDC, rcText);
			} else {
				RECT rcOldPos = pControl->GetPos ();
				pControl->SetPos (rcText);
				pControl->DoPaint (hDC, rcText, nullptr);
				pControl->SetPos (rcOldPos);
			}
		}
	}
}
