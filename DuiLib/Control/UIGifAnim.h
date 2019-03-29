#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#pragma once

namespace DuiLib {
	class UILIB_API CGifAnimUI: public CControlUI {
		enum {
			EVENT_TIEM_ID = 100,
		};
		DECLARE_DUICONTROL (CGifAnimUI)
	public:
		CGifAnimUI (void);
		virtual ~CGifAnimUI (void);

		faw::string_view_t	GetClass () const;
		LPVOID	GetInterface (faw::string_view_t pstrName);
		void	DoInit () override;
		bool	DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void	DoEvent (TEventUI& event);
		void	SetVisible (bool bVisible = true);
		void	SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);
		void	SetBkImage (faw::string_view_t pStrImage);
		faw::string_view_t GetBkImage ();

		void	SetAutoPlay (bool bIsAuto = true);
		bool	IsAutoPlay () const;
		void	SetAutoSize (bool bIsAuto = true);
		bool	IsAutoSize () const;
		void	PlayGif ();
		void	PauseGif ();
		void	StopGif ();

	private:
		void	InitGifImage ();
		void	DeleteGif ();
		void    OnTimer (UINT_PTR idEvent);
		void	DrawFrame (HDC hDC);		// 绘制GIF每帧
		Gdiplus::Image*	LoadGifFromFile (faw::string_view_t pstrGifPath);
		Gdiplus::Image* LoadGifFromMemory (LPVOID pBuf, size_t dwSize);
	private:
		Gdiplus::Image			*m_pGifImage		= nullptr;
		UINT					m_nFrameCount		= 0;		// gif图片总帧数
		UINT					m_nFramePosition	= 0;		// 当前放到第几帧
		Gdiplus::PropertyItem	*m_pPropertyItem	= nullptr;	// 帧与帧之间间隔时间

		faw::String				m_sBkImage;
		bool					m_bIsAutoPlay		= true;		// 是否自动播放gif
		bool					m_bIsAutoSize		= false;	// 是否自动根据图片设置大小
		bool					m_bIsPlaying		= false;
		IStream					*m_pStream			= nullptr;
	};
}

#endif // GifAnimUI_h__