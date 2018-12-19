#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#pragma once

namespace DuiLib {
	class UILIB_API CTabLayoutUI: public CContainerUI {
		DECLARE_DUICONTROL (CTabLayoutUI)
	public:
		CTabLayoutUI ();

		string_view_t GetClass () const;
		LPVOID GetInterface (string_view_t pstrName);

		bool Add (CControlUI* pControl);
		bool AddAt (CControlUI* pControl, int iIndex);
		bool Remove (CControlUI* pControl);
		void RemoveAll ();
		int GetCurSel () const;
		virtual bool SelectItem (int iIndex);
		virtual bool SelectItem (CControlUI* pControl);

		void SetPos (RECT rc, bool bNeedInvalidate = true);

		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);

	protected:
		int m_iCurSel = -1;
	};
}
#endif // __UITABLAYOUT_H__
