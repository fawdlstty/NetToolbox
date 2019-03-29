#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib {
	class UILIB_API CTextUI: public CLabelUI {
		DECLARE_DUICONTROL (CTextUI)
	public:
		CTextUI ();
		virtual ~CTextUI ();

		faw::string_view_t GetClass () const;
		UINT GetControlFlags () const;
		LPVOID GetInterface (faw::string_view_t pstrName);

		faw::String* GetLinkContent (int iIndex);

		void DoEvent (TEventUI& event);
		SIZE EstimateSize (SIZE szAvailable);

		void PaintText (HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int			m_nLinks			= 0;
		RECT		m_rcLinks[MAX_LINK];
		faw::String	m_sLinks[MAX_LINK];
		int			m_nHoverLink		= -1;
	};

} // namespace DuiLib

#endif //__UITEXT_H__