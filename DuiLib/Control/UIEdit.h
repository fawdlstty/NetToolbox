#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

namespace DuiLib {
	class CEditWnd;

	class UILIB_API CEditUI: public CLabelUI {
		DECLARE_DUICONTROL (CEditUI)
		friend class CEditWnd;
	public:
		CEditUI ();

		string_view_t GetClass () const;
		LPVOID GetInterface (string_view_t pstrName);
		UINT GetControlFlags () const;

		void SetEnabled (bool bEnable = true);
		void SetText (string_view_t pstrText);
		void SetMaxChar (UINT uMax);
		UINT GetMaxChar ();
		void SetReadOnly (bool bReadOnly);
		bool IsReadOnly () const;
		void SetPasswordMode (bool bPasswordMode);
		bool IsPasswordMode () const;
		void SetPasswordChar (TCHAR cPasswordChar);
		TCHAR GetPasswordChar () const;
		void SetNumberOnly (bool bNumberOnly);
		bool IsNumberOnly () const;
		int GetWindowStyls () const;

		string_view_t GetNormalImage ();
		void SetNormalImage (string_view_t pStrImage);
		string_view_t GetHotImage ();
		void SetHotImage (string_view_t pStrImage);
		string_view_t GetFocusedImage ();
		void SetFocusedImage (string_view_t pStrImage);
		string_view_t GetDisabledImage ();
		void SetDisabledImage (string_view_t pStrImage);
		void SetNativeEditBkColor (DWORD dwBkColor);
		DWORD GetNativeEditBkColor () const;
		void SetNativeEditTextColor (string_view_t pStrColor);
		DWORD GetNativeEditTextColor () const;

		bool IsAutoSelAll ();
		void SetAutoSelAll (bool bAutoSelAll);
		void SetSel (long nStartChar, long nEndChar);
		void SetSelAll ();
		void SetReplaceSel (string_view_t lpszReplace);

		void SetTipValue (string_view_t pStrTipValue);
		CDuiString GetTipValue ();
		void SetTipValueColor (string_view_t pStrColor);
		DWORD GetTipValueColor ();

		void SetPos (RECT rc, bool bNeedInvalidate = true);
		void Move (SIZE szOffset, bool bNeedInvalidate = true);
		void SetVisible (bool bVisible = true);
		void SetInternVisible (bool bVisible = true);
		SIZE EstimateSize (SIZE szAvailable);
		void DoEvent (TEventUI& event);
		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);

		void PaintStatusImage (HDC hDC);
		void PaintText (HDC hDC);

	protected:
		CEditWnd	*m_pWindow			= nullptr;

		UINT		m_uMaxChar			= 255;
		bool		m_bReadOnly			= false;
		bool		m_bPasswordMode		= false;
		bool		m_bAutoSelAll		= false;
		TCHAR		m_cPasswordChar		= _T ('*');
		UINT		m_uButtonState		= 0;
		CDuiString	m_sNormalImage;
		CDuiString	m_sHotImage;
		CDuiString	m_sFocusedImage;
		CDuiString	m_sDisabledImage;
		CDuiString	m_sTipValue;
		DWORD		m_dwTipValueColor	= 0xFFBAC0C5;
		DWORD		m_dwEditbkColor		= 0xFFFFFFFF;
		DWORD		m_dwEditTextColor	= 0x00000000;
		int			m_iWindowStyls		= 0;
	};
}
#endif // __UIEDIT_H__