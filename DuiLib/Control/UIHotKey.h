#ifndef __UIHOTKEY_H__
#define __UIHOTKEY_H__
#pragma once

namespace DuiLib {
	class CHotKeyUI;

	class UILIB_API CHotKeyWnd: public CWindowWnd {
	public:
		CHotKeyWnd (void);

	public:
		void Init (CHotKeyUI * pOwner);
		RECT CalPos ();
		faw::string_view_t GetWindowClassName () const;
		void OnFinalMessage (HWND hWnd);
		faw::string_view_t GetSuperClassName () const;
		LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	public:
		void SetHotKey (WORD wVirtualKeyCode, WORD wModifiers);
		void GetHotKey (WORD &wVirtualKeyCode, WORD &wModifiers) const;
		DWORD GetHotKey (void) const;
		faw::String GetHotKeyName ();
		void SetRules (WORD wInvalidComb, WORD wModifiers);
		faw::String GetKeyName (UINT vk, BOOL fExtended);
	protected:
		CHotKeyUI * m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};

	class UILIB_API CHotKeyUI: public CLabelUI {
		DECLARE_DUICONTROL (CHotKeyUI)
		friend CHotKeyWnd;
	public:
		CHotKeyUI ();
		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);
		UINT GetControlFlags () const;
		void SetEnabled (bool bEnable = true);
		void SetText (faw::String pstrText);
		faw::string_view_t GetNormalImage ();
		void SetNormalImage (faw::string_view_t pStrImage);
		faw::string_view_t GetHotImage ();
		void SetHotImage (faw::string_view_t pStrImage);
		faw::string_view_t GetFocusedImage ();
		void SetFocusedImage (faw::string_view_t pStrImage);
		faw::string_view_t GetDisabledImage ();
		void SetDisabledImage (faw::string_view_t pStrImage);
		void SetNativeBkColor (DWORD dwBkColor);
		DWORD GetNativeBkColor () const;

		void SetPos (RECT rc);
		void SetVisible (bool bVisible = true);
		void SetInternVisible (bool bVisible = true);
		SIZE EstimateSize (SIZE szAvailable);
		void DoEvent (TEventUI& event);
		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

		void PaintStatusImage (HDC hDC);
		void PaintText (HDC hDC);
	public:
		void GetHotKey (WORD &wVirtualKeyCode, WORD &wModifiers) const;
		DWORD GetHotKey (void) const;
		void SetHotKey (WORD wVirtualKeyCode, WORD wModifiers);

	protected:
		CHotKeyWnd * m_pWindow;
		UINT m_uButtonState;
		faw::String m_sNormalImage;
		faw::String m_sHotImage;
		faw::String m_sFocusedImage;
		faw::String m_sDisabledImage;
		DWORD m_dwHotKeybkColor;

	protected:
		WORD m_wVirtualKeyCode;
		WORD m_wModifiers;
	};
}


#endif