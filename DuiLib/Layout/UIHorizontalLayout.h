#ifndef __UIHORIZONTALLAYOUT_H__
#define __UIHORIZONTALLAYOUT_H__

#pragma once

namespace DuiLib {
	class UILIB_API CHorizontalLayoutUI: public CContainerUI {
		DECLARE_DUICONTROL (CHorizontalLayoutUI)
	public:
		CHorizontalLayoutUI ();

		string_view_t GetClass () const;
		LPVOID GetInterface (string_view_t pstrName);
		UINT GetControlFlags () const;

		void SetSepWidth (int iWidth);
		int GetSepWidth () const;
		void SetSepImmMode (bool bImmediately);
		bool IsSepImmMode () const;
		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);
		void DoEvent (TEventUI& event);

		void SetPos (RECT rc, bool bNeedInvalidate = true);
		void DoPostPaint (HDC hDC, const RECT& rcPaint);
		bool IsDynamic (POINT &pt) const override;

		RECT GetThumbRect (bool bUseNew = false) const;

	protected:
		int		m_iSepWidth		= 0;
		UINT	m_uButtonState	= 0;
		POINT	ptLastMouse		= { 0 };
		RECT	m_rcNewPos;
		bool	m_bImmMode		= false;
	};
}
#endif // __UIHORIZONTALLAYOUT_H__
