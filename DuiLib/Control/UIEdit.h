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

		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);
		UINT GetControlFlags () const;

		void SetEnabled (bool bEnable = true);
		void SetText (faw::String pstrText);
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

		faw::string_view_t GetNormalImage ();
		void SetNormalImage (faw::string_view_t pStrImage);
		faw::string_view_t GetHotImage ();
		void SetHotImage (faw::string_view_t pStrImage);
		faw::string_view_t GetFocusedImage ();
		void SetFocusedImage (faw::string_view_t pStrImage);
		faw::string_view_t GetDisabledImage ();
		void SetDisabledImage (faw::string_view_t pStrImage);
		void SetNativeEditBkColor (DWORD dwBkColor);
		DWORD GetNativeEditBkColor () const;
		void SetNativeEditTextColor (faw::string_view_t pStrColor);
		DWORD GetNativeEditTextColor () const;

		bool IsAutoSelAll ();
		void SetAutoSelAll (bool bAutoSelAll);
		void SetSel (long nStartChar, long nEndChar);
		void SetSelAll ();
		void SetReplaceSel (faw::string_view_t lpszReplace);

		void SetTipValue (faw::string_view_t pStrTipValue);
		faw::String GetTipValue ();
		void SetTipValueColor (faw::string_view_t pStrColor);
		DWORD GetTipValueColor ();

		void SetPos (RECT rc, bool bNeedInvalidate = true);
		void Move (SIZE szOffset, bool bNeedInvalidate = true);
		void SetVisible (bool bVisible = true);
		void SetInternVisible (bool bVisible = true);
		SIZE EstimateSize (SIZE szAvailable);
		void DoEvent (TEventUI& event);
		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

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
		faw::String	m_sNormalImage;
		faw::String	m_sHotImage;
		faw::String	m_sFocusedImage;
		faw::String	m_sDisabledImage;
		faw::String	m_sTipValue;
		DWORD		m_dwTipValueColor	= 0xFFBAC0C5;
		DWORD		m_dwEditbkColor		= 0xFFFFFFFF;
		DWORD		m_dwEditTextColor	= 0x00000000;
		int			m_iWindowStyls		= 0;
	};
}
#endif // __UIEDIT_H__