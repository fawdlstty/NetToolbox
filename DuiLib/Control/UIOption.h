#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DuiLib {
	class UILIB_API COptionUI: public CButtonUI {
		DECLARE_DUICONTROL (COptionUI)
	public:
		COptionUI ();
		virtual ~COptionUI ();

		string_view_t GetClass () const;
		LPVOID GetInterface (string_view_t pstrName);

		void SetManager (CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

		bool Activate ();
		void SetEnabled (bool bEnable = true);

		string_view_t GetSelectedImage ();
		void SetSelectedImage (string_view_t pStrImage);

		string_view_t GetSelectedHotImage ();
		void SetSelectedHotImage (string_view_t pStrImage);

		string_view_t GetSelectedPushedImage ();
		void SetSelectedPushedImage (string_view_t pStrImage);

		void SetSelectedTextColor (DWORD dwTextColor);
		DWORD GetSelectedTextColor ();

		void SetSelectedBkColor (DWORD dwBkColor);
		DWORD GetSelectedBkColor ();

		string_view_t GetSelectedForedImage ();
		void SetSelectedForedImage (string_view_t pStrImage);

		void SetSelectedStateCount (int nCount);
		int GetSelectedStateCount () const;
		virtual string_view_t GetSelectedStateImage ();
		virtual void SetSelectedStateImage (string_view_t pStrImage);

		void SetSelectedFont (int index);
		int GetSelectedFont () const;

		string_view_t GetGroup () const;
		void SetGroup (string_view_t pStrGroupName = _T (""));
		bool IsSelected () const;
		virtual void Selected (bool bSelected, bool bMsg = true);

		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);

		void PaintBkColor (HDC hDC);
		void PaintStatusImage (HDC hDC);
		void PaintForeImage (HDC hDC);
		void PaintText (HDC hDC);

	protected:
		bool		m_bSelected				= false;
		CDuiString	m_sGroupName;

		int			m_iSelectedFont			= -1;

		DWORD		m_dwSelectedBkColor		= 0;
		DWORD		m_dwSelectedTextColor	= 0;

		CDuiString	m_sSelectedImage;
		CDuiString	m_sSelectedHotImage;
		CDuiString	m_sSelectedPushedImage;
		CDuiString	m_sSelectedForeImage;

		int			m_nSelectedStateCount	= 0;
		CDuiString	m_sSelectedStateImage;
	};

	class UILIB_API CCheckBoxUI: public COptionUI {
		DECLARE_DUICONTROL (CCheckBoxUI)
	public:
		CCheckBoxUI ();

	public:
		virtual string_view_t GetClass () const;
		virtual LPVOID GetInterface (string_view_t pstrName);

		void SetCheck (bool bCheck);
		bool GetCheck () const;

	public:
		virtual void SetAttribute (string_view_t pstrName, string_view_t pstrValue);
		void SetAutoCheck (bool bEnable);
		virtual void DoEvent (TEventUI& event);
		virtual void Selected (bool bSelected, bool bMsg = true);

	protected:
		bool m_bAutoCheck;
	};
} // namespace DuiLib

#endif // __UIOPTION_H__