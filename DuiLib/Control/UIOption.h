#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DuiLib {
	class UILIB_API COptionUI: public CButtonUI {
		DECLARE_DUICONTROL (COptionUI)
	public:
		COptionUI ();
		virtual ~COptionUI ();

		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);

		void SetManager (CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

		bool Activate ();
		void SetEnabled (bool bEnable = true);

		faw::string_view_t GetSelectedImage ();
		void SetSelectedImage (faw::string_view_t pStrImage);

		faw::string_view_t GetSelectedHotImage ();
		void SetSelectedHotImage (faw::string_view_t pStrImage);

		faw::string_view_t GetSelectedPushedImage ();
		void SetSelectedPushedImage (faw::string_view_t pStrImage);

		void SetSelectedTextColor (DWORD dwTextColor);
		DWORD GetSelectedTextColor ();

		void SetSelectedBkColor (DWORD dwBkColor);
		DWORD GetSelectedBkColor ();

		faw::string_view_t GetSelectedForedImage ();
		void SetSelectedForedImage (faw::string_view_t pStrImage);

		void SetSelectedStateCount (int nCount);
		int GetSelectedStateCount () const;
		virtual faw::string_view_t GetSelectedStateImage ();
		virtual void SetSelectedStateImage (faw::string_view_t pStrImage);

		void SetSelectedFont (int index);
		int GetSelectedFont () const;

		faw::string_view_t GetGroup () const;
		void SetGroup (faw::string_view_t pStrGroupName = _T (""));
		bool IsSelected () const;
		virtual void Selected (bool bSelected, bool bMsg = true);

		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

		void PaintBkColor (HDC hDC);
		void PaintStatusImage (HDC hDC);
		void PaintForeImage (HDC hDC);
		void PaintText (HDC hDC);

	protected:
		bool		m_bSelected				= false;
		faw::String	m_sGroupName;

		int			m_iSelectedFont			= -1;

		DWORD		m_dwSelectedBkColor		= 0;
		DWORD		m_dwSelectedTextColor	= 0;

		faw::String	m_sSelectedImage;
		faw::String	m_sSelectedHotImage;
		faw::String	m_sSelectedPushedImage;
		faw::String	m_sSelectedForeImage;

		int			m_nSelectedStateCount	= 0;
		faw::String	m_sSelectedStateImage;
	};

	class UILIB_API CCheckBoxUI: public COptionUI {
		DECLARE_DUICONTROL (CCheckBoxUI)
	public:
		CCheckBoxUI ();

	public:
		virtual faw::string_view_t GetClass () const;
		virtual LPVOID GetInterface (faw::string_view_t pstrName);

		void SetCheck (bool bCheck);
		bool GetCheck () const;

	public:
		virtual void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);
		void SetAutoCheck (bool bEnable);
		virtual void DoEvent (TEventUI& event);
		virtual void Selected (bool bSelected, bool bMsg = true);

	protected:
		bool m_bAutoCheck;
	};
} // namespace DuiLib

#endif // __UIOPTION_H__