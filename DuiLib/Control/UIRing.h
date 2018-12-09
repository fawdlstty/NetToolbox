#ifndef __UIROTATE_H__
#define __UIROTATE_H__

#pragma once

namespace DuiLib {
	class CRingUI: public CLabelUI {
		enum {
			RING_TIMERID = 100,
		};
		DECLARE_DUICONTROL (CRingUI)
	public:
		CRingUI ();
		virtual ~CRingUI ();

		string_view_t GetClass () const;
		LPVOID GetInterface (string_view_t pstrName);
		void SetAttribute (string_view_t pstrName, string_view_t pstrValue);
		void SetBkImage (string_view_t pStrImage);
		virtual void DoEvent (TEventUI& event);
		virtual void PaintBkImage (HDC hDC);

	private:
		void InitImage ();
		void DeleteImage ();

	public:
		float			m_fCurAngle		= 0.0f;
		Gdiplus::Image	*m_pBkimage		= nullptr;
	};
}

#endif // __UIROTATE_H__