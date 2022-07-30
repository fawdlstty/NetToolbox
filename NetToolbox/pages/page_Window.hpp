#ifndef __PAGE_WINDOW_HPP__
#define __PAGE_WINDOW_HPP__

#include <chrono>
#include <thread>
#include <Windows.h>

#include "page_base.hpp"

#include "../tools/tool_Gdip.hpp"
#include "../tools/tool_Zoomer.hpp"



class page_FindWnd: public WindowImplBase {
public:
	virtual ~page_FindWnd () {
		if (m_bmp_cover) {
			::DeleteObject (m_bmp_cover);
			m_bmp_cover = NULL;
		}
	}
	LPCTSTR GetWindowClassName () const override { return _T ("NetToolbox"); }
	faw::string_t GetSkinFile () override { return _IT (_T ("cur_findcolor.xml")); }

	void parent_notify_onmove (POINT &pt, faw::string_t text) {
		try {
			//POINT pt { 0 };
			::GetCursorPos (&pt);
			m_find_text->SetText (text);
			POINT pt_dst { pt.x + 40, pt.y };
			if (pt_dst.x + 180 > tool_Zoomer::get_desp_x ())
				pt_dst.x -= 165;
			if (pt_dst.y + 150 > tool_Zoomer::get_desp_y ())
				pt_dst.y -= 110;
			::MoveWindow (GetHWND (), pt_dst.x, pt_dst.y, 120, 102, FALSE);
			RECT rc = m_find_image->GetPos ();
			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;
			if (width == 0 || height == 0)
				return;
			tool_Zoomer::zoom (pt);

			// 判断数据有效时初始化图片
			if (!m_bmp_cover) {
				BITMAPINFO bi { { sizeof (BITMAPINFOHEADER), (LONG) width, (LONG) height, 1, 24, BI_RGB, (DWORD) width * height * 3, 5000, 5000, 0, 0 }, { 0 } };
				void *ptr = nullptr;
				HDC hDeskDC = ::GetDC (NULL);
				m_bmp_cover = ::CreateDIBSection (hDeskDC, &bi, DIB_RGB_COLORS, &ptr, NULL, 0);
				::ReleaseDC (NULL, hDeskDC);
				m_pm.AddImage (_T ("cur_findcolor"), &m_bmp_cover, width, height, false);
				m_find_image->SetBkImage (_T ("cur_findcolor"));
			}

			// 绘制图片
			int level = 4;
			int dest_width = width / level;
			int dest_height = height / level;
			if (pt.x < dest_width / 2) {
				pt.x = dest_width / 2;
			} else if (pt.x > tool_Zoomer::get_real_x () - dest_width / 2) {
				pt.x = tool_Zoomer::get_real_x () - dest_width / 2;
			}
			if (pt.y < dest_height / 2) {
				pt.y = dest_height / 2;
			} else if (pt.y > tool_Zoomer::get_real_y () - dest_height / 2) {
				pt.y = tool_Zoomer::get_real_y () - dest_height / 2;
			}
			pt.x -= dest_width / 2;
			pt.y -= dest_height / 2;
			HDC hDC = ::GetDC (GetHWND ());
			HDC hMemDC = ::CreateCompatibleDC (hDC);
			::SelectObject (hMemDC, m_bmp_cover);
			Gdiplus::Graphics gbmp (hMemDC);
			gbmp.SetInterpolationMode (Gdiplus::InterpolationModeNearestNeighbor);
			gbmp.DrawImage (m_bmp, Gdiplus::Rect (rc.left, rc.top, width, height), pt.x, pt.y, dest_width, dest_height, Gdiplus::UnitPixel);
			RECT rcItem { 0, 0, width, height };
			CRenderEngine::DrawImageString (hMemDC, &m_pm, rcItem, rcItem, _T ("findcolor.png"));
			::DeleteDC (hMemDC);
			::ReleaseDC (GetHWND (), hDC);
			m_find_image->Invalidate ();
			//m_find_image->SetBkImage (_T (""));
			//m_find_image->SetBkImage (_T ("cur_findcolor"));
		} catch (...) {
		}
	}

	LRESULT invoke (std::function<LRESULT ()> f) { return ::SendMessage (m_hWnd, WM_USER + 0x101, 1, (LPARAM) &f); }
	void async_invoke (std::function<LRESULT ()> f) { ::PostMessage (m_hWnd, WM_USER + 0x101, 0, (LPARAM) new decltype (f) (f)); }

	Gdiplus::Bitmap		*m_bmp = nullptr;

private:

	BindControlUI		m_find_image { _T ("find_image"), &m_pm };
	BindTextUI			m_find_text { _T ("find_text"), &m_pm };
	HBITMAP				m_bmp_cover = NULL;
};

class page_Window: public page_base {
public:
	page_Window (NetToolboxWnd *parent): page_base (parent) {
		m_bkimg_findcolor = m_window_findcolor->GetBkImage ();
		m_cur_findcolor = ::LoadCursor (m_parent->get_instance (), MAKEINTRESOURCE (IDC_CURSOR_FINDCOLOR));
		m_find_wnd = new page_FindWnd ();
		m_find_wnd->Create (m_parent->GetHWND (), _IT (_T ("Color extraction")), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		m_find_wnd->ShowWindow (false);

		m_bkimg_detector = m_window_detector->GetBkImage ();
		m_cur_detector = ::LoadCursor (m_parent->get_instance (), MAKEINTRESOURCE (IDC_CURSOR_DETECTOR));

		m_bkimg_zoomer = m_window_zoomer->GetBkImage ();
		m_cur_zoomer = ::LoadCursor (m_parent->get_instance (), MAKEINTRESOURCE (IDC_CURSOR_ZOOMER));
	}
	virtual ~page_Window () {
		set_bmp_screen (nullptr);
		if (m_find_wnd) {
			m_find_wnd->Close ();
			delete m_find_wnd;
		}
	}

	bool OnTimer (TNotifyUI &msg) override {
		if (msg.pSender->GetName () == _T ("window_findcolor")) {
			return true;
		} else if (msg.pSender->GetName () == _T ("window_detector")) {
			m_window_detector->KillTimer ((UINT) *m_window_detector);
			HWND hDeskWnd = ::GetDesktopWindow ();
			HDC hDeskDC = ::GetWindowDC (hDeskWnd);
			int old_rop = ::SetROP2 (hDeskDC, R2_NOTXORPEN);
			POINT pt = { 0 };
			::GetCursorPos (&pt);
			HWND wnd_sel = ::WindowFromPoint (pt);
			RECT rc = { 0 };
			::GetWindowRect (wnd_sel, &rc);
			if (rc.left < 0) rc.left = 0;
			if (rc.top < 0) rc.top = 0;
			HPEN new_pen = ::CreatePen (0, 5, 0);
			HGDIOBJ old_pen = ::SelectObject (hDeskDC, new_pen);
			tool_Zoomer::zoom (rc);
			::Rectangle (hDeskDC, rc.left, rc.top, rc.right, rc.bottom);
			std::thread ([this, hDeskDC, rc, old_rop, old_pen, new_pen, hDeskWnd] () {
				::Sleep (500);
				::Rectangle (hDeskDC, rc.left, rc.top, rc.right, rc.bottom);
				::SetROP2 (hDeskDC, old_rop);
				::SelectObject (hDeskDC, old_pen);
				::DeletePen (new_pen);
				::ReleaseDC (hDeskWnd, hDeskDC);
				if (m_cap_detector)
					m_window_detector->SetTimer ((UINT) *m_window_detector, 500);
			}).detach ();
			return true;
		} else if (msg.pSender->GetName () == _T ("window_zoomer")) {
			HDC hDC = ::GetDC (m_parent->GetHWND ());
			Gdiplus::Graphics g (hDC);
			RECT rc = m_window_zoomhost->GetPos ();
			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;
			POINT pt { 0 };
			::GetCursorPos (&pt);
			tool_Zoomer::zoom (pt);
			int level = 5;
			int dest_width = width / level;
			int dest_height = height / level;
			if (pt.x < dest_width / 2) { pt.x = dest_width / 2; } else if (pt.x > tool_Zoomer::get_real_x () - dest_width / 2) { pt.x = tool_Zoomer::get_real_x () - dest_width / 2; }
			if (pt.y < dest_height / 2) { pt.y = dest_height / 2; } else if (pt.y > tool_Zoomer::get_real_y () - dest_height / 2) { pt.y = tool_Zoomer::get_real_y () - dest_height / 2; }
			pt.x -= dest_width / 2;
			pt.y -= dest_height / 2;
			g.SetInterpolationMode (Gdiplus::InterpolationModeNearestNeighbor);
			g.DrawImage (m_bmp_screen, Gdiplus::Rect (rc.left, rc.top, width, height), pt.x, pt.y, dest_width, dest_height, Gdiplus::UnitPixel);
			::ReleaseDC (m_parent->GetHWND (), hDC);
			return true;
		}
		return false;
	}

	bool OnLButtonDown (POINT &pt) override {
		CControlUI *ctrl = m_parent->find_control (pt);
		if (m_cap_findcolor || m_cap_detector || m_cap_zoomer) {
			return OnLButtonUp (pt);
		} else if (ctrl->GetName () == _T ("window_findcolor")) {
			m_window_tab->SelectItem (0);
			m_cap_findcolor = true;
			::SetCapture (m_parent->GetHWND ());
			m_cur_tmp = ::SetCursor (m_cur_findcolor);
			m_window_findcolor->SetBkImage (_T (""));
			set_bmp_screen (tool_Gdip::capture_screen ());
			m_find_wnd->m_bmp = m_bmp_screen;
			m_find_wnd->ShowWindow ();
			OnMouseMove (pt);
			return true;
		} else if (ctrl->GetName () == _T ("window_detector")) {
			m_window_tab->SelectItem (0);
			m_cap_detector = true;
			::SetCapture (m_parent->GetHWND ());
			m_cur_tmp = ::SetCursor (m_cur_detector);
			m_window_detector->SetBkImage (_T (""));
			set_bmp_screen (tool_Gdip::capture_screen ());
			ctrl->SetTimer ((UINT) ctrl, 500);
			return true;
		} else if (ctrl->GetName () == _T ("window_zoomer")) {
			m_window_tab->SelectItem (1);
			m_cap_zoomer = true;
			::SetCapture (m_parent->GetHWND ());
			m_cur_tmp = ::SetCursor (m_cur_zoomer);
			m_window_zoomer->SetBkImage (_T (""));
			set_bmp_screen (tool_Gdip::capture_screen ());
			ctrl->SetTimer ((UINT) ctrl, 70);
			return true;
		}
		return false;
	}
	bool OnLButtonUp (POINT &pt) override {
		if (m_cap_findcolor) {
			m_cap_findcolor = false;
			::ReleaseCapture ();
			m_window_findcolor->SetBkImage (m_bkimg_findcolor.data ());
			m_find_wnd->ShowWindow (false);
		} else if (m_cap_detector) {
			m_cap_detector = false;
			::ReleaseCapture ();
			m_window_detector->KillTimer ((UINT) *m_window_detector);
			m_window_detector->SetBkImage (m_bkimg_detector.data ());

			//POINT pt { 0 };
			::GetCursorPos (&pt);
			// 获取颜色
			Gdiplus::Color _color;
			m_bmp_screen->GetPixel (pt.x, pt.y, &_color);
			m_window_color->SetBkColor (_color.GetValue () | 0xFF000000);
			faw::string_t str = std::format (_T ("#{:06} ({}, {}, {})"), _color.GetValue (), _color.GetR (), _color.GetG (), _color.GetB ());
			m_window_txt_color->SetText (str.data ());
			// 获取窗口句柄
			HWND hDestWnd = ::WindowFromPoint (pt);
			str = std::format (_T ("{:X}"), (void*) hDestWnd);
			m_window_txt_handle->SetText (str.data ());
			// 获取窗口位置
			RECT rcDest { 0 };
			::GetWindowRect (hDestWnd, &rcDest);
			str = std::format (_T ("pos ({}, {}), size ({}, {})"), rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top);
			m_window_txt_pos->SetText (str.data ());
			// 获取窗口标题
			TCHAR wnd_text[MAX_PATH] = { 0 };
			::GetWindowText (hDestWnd, wnd_text, MAX_PATH);
			m_window_txt_title->SetText (wnd_text);
			// 窗口链
			//HWND hPrevWnd = ::GetNextWindow (hDestWnd, GW_HWNDPREV);
			//HWND hNextWnd = ::GetNextWindow (hDestWnd, GW_HWNDNEXT);
			//HWND hChildWnd = ::GetNextWindow (hDestWnd, GW_CHILD);
			HWND hParentWnd = ::GetNextWindow (hDestWnd, GW_OWNER);
			str = std::format (_T ("{:X}"), (void *) hParentWnd);
			m_window_txt_parent->SetText (str.data ());
			// 窗口样式
			LONG style = ::GetWindowLong (hDestWnd, GWL_STYLE);
			str = std::format (_T ("0x{:08X}"), style);
			m_window_txt_style->SetText (str.data ());
			LONG exstyle = ::GetWindowLong (hDestWnd, GWL_EXSTYLE);
			str = std::format (_T ("0x{:08X}"), exstyle);
			m_window_txt_exstyle->SetText (str.data ());
			// 获取窗口类
			TCHAR class_text[MAX_PATH] = { 0 };
			::GetClassName (hDestWnd, class_text, MAX_PATH);
			m_window_txt_wndclass->SetText (class_text);
			return true;
		} else if (m_cap_zoomer) {
			m_cap_zoomer = false;
			::ReleaseCapture ();
			m_window_zoomer->KillTimer ((UINT) *m_window_zoomer);
			m_window_zoomer->SetBkImage (m_bkimg_zoomer.data ());
			return true;
		}
		return false;
	}

	bool OnRButtonDown (POINT &pt) override {
		if (m_cap_findcolor || m_cap_detector || m_cap_zoomer) {
			return OnLButtonUp (pt);
		}
		return false;
	}
	bool OnRButtonUp (POINT &pt) override {
		if (m_cap_findcolor || m_cap_detector || m_cap_zoomer) {
			return OnLButtonUp (pt);
		}
		return false;
	}

	bool OnMouseMove (POINT &pt) override {
		if (m_cap_findcolor) {
			//POINT pt { 0 };
			::GetCursorPos (&pt);
			tool_Zoomer::zoom (pt);
			// 获取颜色
			Gdiplus::Color _color;
			m_bmp_screen->GetPixel (pt.x, pt.y, &_color);
			m_window_color->SetBkColor (_color.GetValue () | 0xFF000000);
			faw::string_t str = std::format (_T ("#{:06X} ({}, {}, {})"), _color.GetValue () & 0xFFFFFF, _color.GetR (), _color.GetG (), _color.GetB ());
			m_window_txt_color->SetText (str.data ());
			//
			str = std::format (_T ("RGB ({}, {}, {})"), _color.GetR (), _color.GetG (), _color.GetB ());
			m_find_wnd->parent_notify_onmove (pt, str);
			return true;
		}
		return false;
	}

protected:
	void set_bmp_screen (Gdiplus::Bitmap *bmp) {
		if (m_bmp_screen)
			delete m_bmp_screen;
		m_bmp_screen = bmp;
	}

	HCURSOR				m_cur_tmp				= ::LoadCursor (NULL, IDC_ARROW);
	Gdiplus::Bitmap		*m_bmp_screen			= nullptr;

	BindContainerUI		m_window_findcolor { _T ("window_findcolor") };
	faw::string_t			m_bkimg_findcolor		= _T ("");
	bool				m_cap_findcolor			= false;
	HCURSOR				m_cur_findcolor;
	page_FindWnd		*m_find_wnd				= nullptr;

	BindContainerUI		m_window_detector { _T ("window_detector") };
	faw::string_t			m_bkimg_detector		= _T ("");
	bool				m_cap_detector			= false;
	HCURSOR				m_cur_detector;

	BindContainerUI		m_window_zoomer { _T ("window_zoomer") };
	faw::string_t			m_bkimg_zoomer			= _T ("");
	bool				m_cap_zoomer			= false;
	HCURSOR				m_cur_zoomer;

	BindTabLayoutUI		m_window_tab { _T ("window_tab") };
	BindContainerUI		m_window_color { _T ("window_color") };
	BindEditUI			m_window_txt_color { _T ("window_txt_color") };
	BindEditUI			m_window_txt_handle { _T ("window_txt_handle") };
	BindEditUI			m_window_txt_pos { _T ("window_txt_pos") };
	BindEditUI			m_window_txt_title { _T ("window_txt_title") };
	BindEditUI			m_window_txt_parent { _T ("window_txt_parent") };
	BindEditUI			m_window_txt_style { _T ("window_txt_style") };
	BindEditUI			m_window_txt_exstyle { _T ("window_txt_exstyle") };
	BindEditUI			m_window_txt_wndclass { _T ("window_txt_wndclass") };
	BindContainerUI		m_window_zoomhost { _T ("window_zoomhost") };
};

#endif //__PAGE_WINDOW_HPP__
