#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib {
	class UILIB_API CChildLayoutUI: public CContainerUI {
		DECLARE_DUICONTROL (CChildLayoutUI)
	public:
		CChildLayoutUI ();

		void Init ();
		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);
		void SetChildLayoutXML (CDuiString pXML);
		CDuiString GetChildLayoutXML ();
		virtual LPVOID GetInterface (string_view_t pstrName);
		virtual string_view_t GetClass () const;

	private:
		CDuiString m_pstrXMLFile;
	};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
