#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib {
	class UILIB_API CChildLayoutUI: public CContainerUI {
		DECLARE_DUICONTROL (CChildLayoutUI)
	public:
		CChildLayoutUI ();

		void Init ();
		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);
		void SetChildLayoutXML (faw::String pXML);
		faw::String GetChildLayoutXML ();
		virtual LPVOID GetInterface (faw::string_view_t pstrName);
		virtual faw::string_view_t GetClass () const;

	private:
		faw::String m_pstrXMLFile;
	};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
