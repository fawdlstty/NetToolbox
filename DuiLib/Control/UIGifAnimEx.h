#ifndef GifAnimUIEX_h__
#define GifAnimUIEX_h__
#pragma once
/* write by wangji 2016.03.16
** 解决多个gif控件在gdi+环境下占用CPU过高的问题，本类采用ximage替代
** 注意：使用的时候在预编译头文件中包含UIlib.h前先定义宏USE_XIMAGE_EFFECT
** #define USE_XIMAGE_EFFECT
** #include "UIlib.h"
*/
#ifdef USE_XIMAGE_EFFECT
namespace DuiLib {
	class CLabelUI;

	class UILIB_API CGifAnimExUI: public CLabelUI {
		DECLARE_DUICONTROL (CGifAnimExUI)
	public:
		CGifAnimExUI (void);
		virtual ~CGifAnimExUI (void);
	public:
		virtual faw::string_view_t	GetClass () const;
		virtual LPVOID	GetInterface (faw::string_view_t pstrName);
		virtual void Init ();
		virtual void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);
		virtual void SetVisible (bool bVisible = true);
		virtual void SetInternVisible (bool bVisible = true);
		virtual bool DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		virtual void DoEvent (TEventUI& event);
	public:
		void StartAnim ();
		void StopAnim ();
	protected:
		struct Imp;
		Imp* m_pImp;
	};
}
#endif //USE_XIMAGE_EFFECT
#endif // GifAnimUIEx_h__
