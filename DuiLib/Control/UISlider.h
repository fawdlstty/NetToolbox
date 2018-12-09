#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace DuiLib {
	class UILIB_API CSliderUI: public CProgressUI {
		DECLARE_DUICONTROL (CSliderUI)
	public:
		CSliderUI ();

		string_view_t GetClass () const;
		UINT GetControlFlags () const;
		LPVOID GetInterface (string_view_t pstrName);

		void SetEnabled (bool bEnable = true);

		int GetChangeStep ();
		void SetChangeStep (int step);
		void SetThumbSize (SIZE szXY);
		RECT GetThumbRect () const;
		string_view_t GetThumbImage () const;
		void SetThumbImage (string_view_t pStrImage);
		string_view_t GetThumbHotImage () const;
		void SetThumbHotImage (string_view_t pStrImage);
		string_view_t GetThumbPushedImage () const;
		void SetThumbPushedImage (string_view_t pStrImage);

		void DoEvent (TEventUI& event);
		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);
		void PaintForeImage (HDC hDC);

		void SetValue (int nValue);
		void SetCanSendMove (bool bCanSend);
		bool GetCanSendMove () const;
	protected:
		SIZE		m_szThumb		= { 0 };
		UINT		m_uButtonState	= 0;
		int			m_nStep			= 1;

		CDuiString	m_sThumbImage;
		CDuiString	m_sThumbHotImage;
		CDuiString	m_sThumbPushedImage;

		CDuiString	m_sImageModify;
		bool		m_bSendMove		= false;
	};
}

#endif // __UISLIDER_H__