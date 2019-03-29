#ifndef __PAGE_GIF_HPP__
#define __PAGE_GIF_HPP__

#include <vector>
#include <thread>
#include <gdiplus.h>

#include "page_base.hpp"
#include "../tools/tool_String.hpp"
#include "../tools/tool_Gdip.hpp"
#include "../tools/tool_Zoomer.hpp"



class page_GifWnd: public WindowImplBase {
public:
	page_GifWnd (std::vector<Gdiplus::Image*> *ppvgif, volatile size_t *_pdelay): m_pvgif (ppvgif), m_pdelay (_pdelay) {}
	virtual ~page_GifWnd () {
		while (m_vgif.size ()) {
			delete m_vgif[0];
			m_vgif.erase (m_vgif.begin ());
		}
	}

	faw::string_view_t GetWindowClassName () const override { return _T ("NetToolbox"); }
	faw::string_view_t GetSkinFile () override { return _T ("scr2gif.xml"); }
	void InitWindow () override {
		m_init = true;
		RECT rc { 0 };
		::GetWindowRect (GetHWND (), &rc);
		::SetWindowPos (GetHWND (), HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOREDRAW);
	}

	LRESULT OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override {
		if (m_init) {
			POINT pt = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
			m_gifw_width->SetText (faw::String::format (_T ("%d"), pt.x - 24));
			m_gifw_height->SetText (faw::String::format (_T ("%d"), pt.y - 64));
		}
		return WindowImplBase::OnSize (uMsg, wParam, lParam, bHandled);
	}

	void OnClick (TNotifyUI& msg) override {
		 faw::String name = msg.pSender->GetName ();
		if (name == _T ("gifw_ok")) {
			if (m_run) {
				m_run = false;
				m_thread.join ();
			}
			while (m_pvgif->size () > 0) {
				delete (*m_pvgif)[0];
				m_pvgif->erase (m_pvgif->begin ());
			}
			*m_pdelay = m_delay;
			m_pvgif->assign (m_vgif.cbegin (), m_vgif.cend ());
			m_vgif.clear ();
			Close ();
		} else if (name == _T ("gifw_cancel")) {
			if (m_run) {
				m_run = false;
				m_thread.join ();
			}
			Close ();
		} else if (name == _T ("gifw_start")) {
			m_gifw_ok->SetEnabled (false);
			m_gifw_start->SetVisible (false);
			m_gifw_fps->SetReadOnly (true);
			m_gifw_width->SetReadOnly (true);
			m_gifw_height->SetReadOnly (true);
			RECT sz_box { 0, 0, 0, 0 };
			m_pm.SetSizeBox (sz_box);
			m_run = true;
			m_delay = (size_t) (100.0 / _ttoi (m_gifw_fps->GetText ().c_str ()) + 0.5) * 10;
			size_t _width = _ttoi (m_gifw_width->GetText ().c_str ());
			_width = tool_Zoomer::zoom_x (_width);
			size_t _height = _ttoi (m_gifw_height->GetText ().c_str ());
			_height = tool_Zoomer::zoom_y (_height);
			m_thread = std::thread (&page_GifWnd::_record_thread, this, m_delay, _width, _height);
			m_gifw_stop->SetVisible (true);
		} else if (name == _T ("gifw_stop")) {
			m_gifw_stop->SetVisible (false);
			m_run = false;
			m_thread.join ();
			RECT sz_box { 5, 5, 5, 5 };
			m_pm.SetSizeBox (sz_box);
			m_gifw_fps->SetReadOnly (false);
			m_gifw_width->SetReadOnly (false);
			m_gifw_height->SetReadOnly (false);
			m_gifw_start->SetVisible (true);
			m_gifw_ok->SetEnabled (m_vgif.size () > 0);
		}
	}

	void OnTextChanged (TNotifyUI &msg) override {
		 faw::String name = msg.pSender->GetName ();
		if (name == _T ("gifw_width")) {
			SIZE sz = m_pm.GetInitSize ();
			sz.cx = _ttoi (m_gifw_width->GetText ().c_str ()) + 24;
			//m_pm.SetInitSize (sz.cx, sz.cy);
			::SetWindowPos (GetHWND (), nullptr, 0, 0, sz.cx, sz.cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		} else if (name == _T ("gifw_height")) {
			SIZE sz = m_pm.GetInitSize ();
			sz.cy = _ttoi (m_gifw_height->GetText ().c_str ()) + 64;
			//m_pm.SetInitSize (sz.cx, sz.cy);
			::SetWindowPos (GetHWND (), nullptr, 0, 0, sz.cx, sz.cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		} else {
			WindowImplBase::OnTextChanged (msg);
		}
	}

private:
	void _record_thread (size_t m_delay, size_t _width, size_t _height) {
		while (m_vgif.size () > 0) {
			delete m_vgif[0];
			m_vgif.erase (m_vgif.begin ());
		}
		auto tp = std::chrono::system_clock::now ();
		while (m_run) {
			// 屏幕截图
			RECT rect = { 0 };
			GetWindowRect (GetHWND (), &rect);
			rect.left += 12;
			rect.top += 32;
			tool_Zoomer::zoom (rect);
			Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap ((INT) _width, (INT) _height, PixelFormat24bppRGB);
			{
				Gdiplus::Graphics g (bmp);
				HDC hDeskDC = ::GetDC (NULL);
				HDC hGDC = g.GetHDC ();
				::BitBlt (hGDC, 0, 0, (int) _width, (int) _height, hDeskDC, rect.left, rect.top, SRCCOPY);
				g.ReleaseHDC (hGDC);
				::ReleaseDC (NULL, hDeskDC);
			}
			tool_Zoomer::unzoom (&bmp);
			Gdiplus::Graphics g (bmp);

			// 绘制鼠标指针
			CURSORINFO ci { sizeof (CURSORINFO) };
			::GetCursorInfo (&ci);
			tool_Zoomer::zoom (ci.ptScreenPos);
			Gdiplus::Bitmap *bmp_cur = tool_Gdip::cursor_to_bmp (ci.hCursor);
			SIZE sz_cur { (LONG) bmp_cur->GetWidth (), (LONG) bmp_cur->GetHeight () };
			tool_Zoomer::unzoom (sz_cur);
			RECT _rect = { ci.ptScreenPos.x - rect.left, ci.ptScreenPos.y - rect.top, sz_cur.cx, sz_cur.cy };
			tool_Zoomer::unzoom (_rect);
			g.DrawImage (bmp_cur, Gdiplus::Rect (_rect.left, _rect.top, _rect.right, _rect.bottom), 0, 0, sz_cur.cx, sz_cur.cy, Gdiplus::UnitPixel);
			delete bmp_cur;

			// 下一轮循环
			m_vgif.push_back (bmp);
			tp += std::chrono::milliseconds (m_delay);
			std::this_thread::sleep_until (tp);
		}
	}

	BindEditUI						m_gifw_fps { _T ("gifw_fps"), &m_pm };
	BindEditUI						m_gifw_width { _T ("gifw_width"), &m_pm };
	BindEditUI						m_gifw_height { _T ("gifw_height"), &m_pm };
	BindButtonUI					m_gifw_start { _T ("gifw_start"), &m_pm };
	BindButtonUI					m_gifw_stop { _T ("gifw_stop"), &m_pm };
	BindButtonUI					m_gifw_ok { _T ("gifw_ok"), &m_pm };
	bool							m_init = false;
	//
	std::thread						m_thread;
	volatile bool					m_run = false;
	std::vector<Gdiplus::Image*>	m_vgif;
	std::vector<Gdiplus::Image*>	*m_pvgif = nullptr;
	volatile size_t					m_delay;
	volatile size_t					*m_pdelay = nullptr;
};



class page_Gif: public page_base {
public:
	page_Gif (NetToolboxWnd *parent): page_base (parent) {
		faw::String path = faw::Directory::get_current_path ();
		m_gif_path->SetText (faw::String::format (_T ("%c:\\record.gif"), path [0]));
	}
	virtual ~page_Gif () = default;

	bool OnClick (TNotifyUI& msg) override {
		 faw::String name = msg.pSender->GetName ();
		if (name == _T ("gif_showwnd")) {
			m_run = false;
			m_parent->ShowWindow (false);
			page_GifWnd wnd_child { &m_vgif, &m_delay };
			wnd_child.Create (m_parent->GetHWND (), _T ("GIF录制"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
			wnd_child.CenterWindow ();
			wnd_child.ShowModal ();
			m_parent->ShowWindow (true);
			bool _recorded = m_vgif.size () > 0;
			m_gif_save->SetEnabled (_recorded);
			if (_recorded) {
				if (m_thread.joinable ())
					m_thread.join ();
				m_gif_preview->SetText (_T (""));
				m_run = true;
				m_thread = std::thread (&page_Gif::_preview_thread, this);
			}
			return true;
		} else if (name == _T ("gif_save")) {
			faw::String file = m_gif_path->GetText ();
			if (faw::Directory::exist (file)) {
				faw::String info = faw::String::format (_T ("以下文件已存在，是否覆盖？\n%s"), file.c_str ());
				if (IDOK != ::MessageBox (NULL, info.c_str (), _T ("提示"), MB_ICONQUESTION | MB_OKCANCEL))
					return true;
			}
			if (tool_Gdip::gdip_save_animation (m_vgif, file.c_str (), m_delay))
				m_parent->show_status (NetToolboxWnd::StatusIcon::Ok, _T ("GIF图导出成功！"));
			else
				m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _T ("GIF图导出失败！"));
			return true;
		}
		return false;
	}

protected:
	void _preview_thread () {
		auto tp = std::chrono::system_clock::now ();
		size_t i = 0;
		while (m_run) {
			Gdiplus::Image *img = m_vgif[i++ % m_vgif.size ()];
			m_parent->invoke ([this, img] () -> LRESULT {
				if (m_gif_preview->IsVisible ()) {
					HDC hDC = ::GetDC (m_parent->GetHWND ());
					Gdiplus::Graphics g (hDC);
					RECT rc = m_gif_preview->GetPos ();
					int wnd_width = rc.right - rc.left;
					int wnd_height = rc.bottom - rc.top;
					int img_width = (int) img->GetWidth ();
					int img_height = (int) img->GetHeight ();
					if (img_width > wnd_width) {
						img_height = img_height * wnd_width / img_width;
						img_width = wnd_width;
					}
					if (img_height > wnd_height) {
						img_width = img_width * wnd_height / img_height;
						img_height = wnd_height;
					}
					g.DrawImage (img, Gdiplus::Rect (rc.left, rc.top, img_width, img_height), 0, 0, img->GetWidth (), img->GetHeight (), Gdiplus::UnitPixel);
					::ReleaseDC (m_parent->GetHWND (), hDC);
				}
				return 0;
			});
			tp += std::chrono::milliseconds (m_delay);
			std::this_thread::sleep_until (tp);
		}
	}

	BindEditUI						m_gif_path { _T ("gif_path") };
	BindButtonUI					m_gif_save { _T ("gif_save") };
	BindTextUI						m_gif_preview { _T ("gif_preview") };

	std::vector<Gdiplus::Image*>	m_vgif;
	volatile size_t					m_delay = 50;
	volatile bool					m_run = false;
	std::thread						m_thread;
};

#endif //__PAGE_GIF_HPP__
