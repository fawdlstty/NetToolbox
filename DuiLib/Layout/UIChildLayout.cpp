#include "StdAfx.h"
#include "UIChildLayout.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL (CChildLayoutUI)

	CChildLayoutUI::CChildLayoutUI () {}

	void CChildLayoutUI::Init () {
		if (!m_pstrXMLFile.empty ()) {
			CDialogBuilder builder;
			CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create (m_pstrXMLFile, (UINT) 0, nullptr, m_pManager));
			if (pChildWindow) {
				this->Add (pChildWindow);
			} else {
				this->RemoveAll ();
			}
		}
	}

	void CChildLayoutUI::SetAttribute (string_view_t pstrName, string_view_t pstrValue) {
		if (pstrName == _T ("xmlfile"))
			SetChildLayoutXML (pstrValue);
		else
			CContainerUI::SetAttribute (pstrName, pstrValue);
	}

	void CChildLayoutUI::SetChildLayoutXML (CDuiString pXML) {
		m_pstrXMLFile = pXML;
	}

	CDuiString CChildLayoutUI::GetChildLayoutXML () {
		return m_pstrXMLFile;
	}

	LPVOID CChildLayoutUI::GetInterface (string_view_t pstrName) {
		if (pstrName == DUI_CTRL_CHILDLAYOUT) return static_cast<CChildLayoutUI*>(this);
		return CControlUI::GetInterface (pstrName);
	}

	string_view_t CChildLayoutUI::GetClass () const {
		return _T ("ChildLayoutUI");
	}
} // namespace DuiLib
