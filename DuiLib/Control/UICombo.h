#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CComboWnd;

	class UILIB_API CComboUI: public CContainerUI, public IListOwnerUI {
		DECLARE_DUICONTROL (CComboUI)
		friend class CComboWnd;
	public:
		CComboUI ();

		string_view_t GetClass () const;
		LPVOID GetInterface (string_view_t pstrName);

		void DoInit ();
		UINT GetControlFlags () const;

		CDuiString GetText () const;
		void SetText (string_view_t pstrText) override;
		void SetEnabled (bool bEnable = true);

		void SetTextStyle (UINT uStyle);
		UINT GetTextStyle () const;
		void SetTextColor (DWORD dwTextColor);
		DWORD GetTextColor () const;
		void SetDisabledTextColor (DWORD dwTextColor);
		DWORD GetDisabledTextColor () const;
		void SetFont (int index);
		int GetFont () const;
		RECT GetTextPadding () const;
		void SetTextPadding (RECT rc);
		bool IsShowHtml ();
		void SetShowHtml (bool bShowHtml = true);
		bool IsShowShadow ();
		void SetShowShadow (bool bShow = true);

		string_view_t GetDropBoxAttributeList ();
		void SetDropBoxAttributeList (string_view_t pstrList);
		SIZE GetDropBoxSize () const;
		void SetDropBoxSize (SIZE szDropBox);

		UINT GetListType ();
		TListInfoUI* GetListInfo ();
		int GetCurSel () const;
		bool SelectItem (int iIndex, bool bTakeFocus = false);
		bool SelectMultiItem (int iIndex, bool bTakeFocus = false);
		bool UnSelectItem (int iIndex, bool bOthers = false);
		bool SetItemIndex (CControlUI* pControl, int iIndex);

		bool Add (CControlUI* pControl);
		bool AddAt (CControlUI* pControl, int iIndex);
		bool Remove (CControlUI* pControl);
		bool RemoveAt (int iIndex);
		void RemoveAll ();

		bool Activate ();

		string_view_t GetNormalImage () const;
		void SetNormalImage (string_view_t pStrImage);
		string_view_t GetHotImage () const;
		void SetHotImage (string_view_t pStrImage);
		string_view_t GetPushedImage () const;
		void SetPushedImage (string_view_t pStrImage);
		string_view_t GetFocusedImage () const;
		void SetFocusedImage (string_view_t pStrImage);
		string_view_t GetDisabledImage () const;
		void SetDisabledImage (string_view_t pStrImage);

		bool GetScrollSelect ();
		void SetScrollSelect (bool bScrollSelect);

		void SetItemFont (int index);
		void SetItemTextStyle (UINT uStyle);
		RECT GetItemTextPadding () const;
		void SetItemTextPadding (RECT rc);
		DWORD GetItemTextColor () const;
		void SetItemTextColor (DWORD dwTextColor);
		DWORD GetItemBkColor () const;
		void SetItemBkColor (DWORD dwBkColor);
		string_view_t GetItemBkImage () const;
		void SetItemBkImage (string_view_t pStrImage);
		bool IsAlternateBk () const;
		void SetAlternateBk (bool bAlternateBk);
		DWORD GetSelectedItemTextColor () const;
		void SetSelectedItemTextColor (DWORD dwTextColor);
		DWORD GetSelectedItemBkColor () const;
		void SetSelectedItemBkColor (DWORD dwBkColor);
		string_view_t GetSelectedItemImage () const;
		void SetSelectedItemImage (string_view_t pStrImage);
		DWORD GetHotItemTextColor () const;
		void SetHotItemTextColor (DWORD dwTextColor);
		DWORD GetHotItemBkColor () const;
		void SetHotItemBkColor (DWORD dwBkColor);
		string_view_t GetHotItemImage () const;
		void SetHotItemImage (string_view_t pStrImage);
		DWORD GetDisabledItemTextColor () const;
		void SetDisabledItemTextColor (DWORD dwTextColor);
		DWORD GetDisabledItemBkColor () const;
		void SetDisabledItemBkColor (DWORD dwBkColor);
		string_view_t GetDisabledItemImage () const;
		void SetDisabledItemImage (string_view_t pStrImage);
		DWORD GetItemLineColor () const;
		void SetItemLineColor (DWORD dwLineColor);
		bool IsItemShowHtml ();
		void SetItemShowHtml (bool bShowHtml = true);

		SIZE EstimateSize (SIZE szAvailable);
		void SetPos (RECT rc, bool bNeedInvalidate = true);
		void Move (SIZE szOffset, bool bNeedInvalidate = true);
		void DoEvent (TEventUI& event);
		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);

		bool DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void PaintText (HDC hDC);
		void PaintStatusImage (HDC hDC);

	protected:
		CComboWnd	*m_pWindow				= nullptr;

		int			m_iCurSel				= -1;
		DWORD		m_dwTextColor			= 0;
		DWORD		m_dwDisabledTextColor	= 0;
		int			m_iFont					= -1;
		UINT		m_uTextStyle;
		RECT		m_rcTextPadding			= { 0 };
		bool		m_bShowHtml				= false;
		bool		m_bShowShadow;
		CDuiString	m_sDropBoxAttributes;
		SIZE		m_szDropBox				= { 0, 150 };
		UINT		m_uButtonState			= 0;

		CDuiString	m_sNormalImage;
		CDuiString	m_sHotImage;
		CDuiString	m_sPushedImage;
		CDuiString	m_sFocusedImage;
		CDuiString	m_sDisabledImage;

		bool		m_bScrollSelect			= true;
		TListInfoUI	m_ListInfo;
	};

} // namespace DuiLib

#endif // __UICOMBO_H__
