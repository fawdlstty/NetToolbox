#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib {
	class UILIB_API CButtonUI: public CLabelUI {
		DECLARE_DUICONTROL (CButtonUI)

	public:
		CButtonUI ();

		string_view_t GetClass () const;
		LPVOID GetInterface (string_view_t pstrName);
		UINT GetControlFlags () const;

		bool Activate ();
		void SetEnabled (bool bEnable = true);
		void DoEvent (TEventUI& event);

		virtual string_view_t GetNormalImage ();
		virtual void SetNormalImage (string_view_t pStrImage);
		virtual string_view_t GetHotImage ();
		virtual void SetHotImage (string_view_t pStrImage);
		virtual string_view_t GetPushedImage ();
		virtual void SetPushedImage (string_view_t pStrImage);
		virtual string_view_t GetFocusedImage ();
		virtual void SetFocusedImage (string_view_t pStrImage);
		virtual string_view_t GetDisabledImage ();
		virtual void SetDisabledImage (string_view_t pStrImage);
		virtual string_view_t GetHotForeImage ();
		virtual void SetHotForeImage (string_view_t pStrImage);
		void SetStateCount (int nCount);
		int GetStateCount () const;
		virtual string_view_t GetStateImage ();
		virtual void SetStateImage (string_view_t pStrImage);

		void BindTabIndex (int _BindTabIndex);
		void BindTabLayoutName (string_view_t _TabLayoutName);
		void BindTriggerTabSel (int _SetSelectIndex = -1);
		void RemoveBindTabIndex ();
		int	 GetBindTabLayoutIndex ();
		string_view_t GetBindTabLayoutName ();

		void SetHotFont (int index);
		int GetHotFont () const;
		void SetPushedFont (int index);
		int GetPushedFont () const;
		void SetFocusedFont (int index);
		int GetFocusedFont () const;

		void SetHotBkColor (DWORD dwColor);
		DWORD GetHotBkColor () const;
		void SetPushedBkColor (DWORD dwColor);
		DWORD GetPushedBkColor () const;
		void SetDisabledBkColor (DWORD dwColor);
		DWORD GetDisabledBkColor () const;
		void SetHotTextColor (DWORD dwColor);
		DWORD GetHotTextColor () const;
		void SetPushedTextColor (DWORD dwColor);
		DWORD GetPushedTextColor () const;
		void SetFocusedTextColor (DWORD dwColor);
		DWORD GetFocusedTextColor () const;
		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);

		void PaintText (HDC hDC);

		void PaintBkColor (HDC hDC);
		void PaintStatusImage (HDC hDC);
		void PaintForeImage (HDC hDC);

	protected:
		UINT		m_uButtonState			= 0;

		int			m_iHotFont				= -1;
		int			m_iPushedFont			= -1;
		int			m_iFocusedFont			= -1;

		DWORD		m_dwHotBkColor			= 0;
		DWORD		m_dwPushedBkColor		= 0;
		DWORD		m_dwDisabledBkColor		= 0;
		DWORD		m_dwHotTextColor		= 0;
		DWORD		m_dwPushedTextColor		= 0;
		DWORD		m_dwFocusedTextColor	= 0;

		CDuiString	m_sNormalImage;
		CDuiString	m_sHotImage;
		CDuiString	m_sHotForeImage;
		CDuiString	m_sPushedImage;
		CDuiString	m_sPushedForeImage;
		CDuiString	m_sFocusedImage;
		CDuiString	m_sDisabledImage;
		int			m_nStateCount			= 0;
		CDuiString	m_sStateImage;

		int			m_iBindTabIndex			= -1;
		CDuiString	m_sBindTabLayoutName;
	};

}	// namespace DuiLib

#endif // __UIBUTTON_H__