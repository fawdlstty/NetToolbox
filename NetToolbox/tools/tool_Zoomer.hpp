////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Zoomer
// Description: 屏幕缩放工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_ZOOMER_HPP__
#define __TOOL_ZOOMER_HPP__

#include <cmath>

#include <Windows.h>
#include <gdiplus.h>



class tool_Zoomer {
public:
	// 获取显示器x轴像素宽度
	static int get_desp_x () { return get_zoomer ().m_desp_x; }
	// 获取显示器y轴像素宽度
	static int get_desp_y () { return get_zoomer ().m_desp_y; }
	// 获取实际x轴像素宽度
	static int get_real_x () { return get_zoomer ().m_real_x; }
	// 获取实际y轴像素宽度
	static int get_real_y () { return get_zoomer ().m_real_y; }

	// 缩放x轴
	static size_t zoom_x (size_t x) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return x;
		return (size_t) (x * zoomer.m_scale_x + 0.5);
	}

	// 缩放y轴
	static size_t zoom_y (size_t y) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return y;
		return (size_t) (y * zoomer.m_scale_y + 0.5);
	}

	// 缩放POINT
	static void zoom (POINT &o) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return;
		o.x = (decltype (o.x)) (o.x * zoomer.m_scale_x + 0.5);
		o.y = (decltype (o.y)) (o.y * zoomer.m_scale_y + 0.5);
	}

	// 缩放SIZE
	static void zoom (SIZE &o) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return;
		o.cx = (decltype (o.cx)) (o.cx * zoomer.m_scale_x + 0.5);
		o.cy = (decltype (o.cy)) (o.cy * zoomer.m_scale_y + 0.5);
	}

	// 反缩放SIZE
	static void unzoom (SIZE &o) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return;
		o.cx = (decltype (o.cx)) (o.cx / zoomer.m_scale_x + 0.5);
		o.cy = (decltype (o.cy)) (o.cy / zoomer.m_scale_y + 0.5);
	}

	// 缩放RECT
	static void zoom (RECT &o) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return;
		o.left = (decltype (o.left)) (o.left * zoomer.m_scale_x + 0.5);
		o.top = (decltype (o.top)) (o.top * zoomer.m_scale_y + 0.5);
		o.right = (decltype (o.right)) (o.right * zoomer.m_scale_x + 0.5);
		o.bottom = (decltype (o.bottom)) (o.bottom * zoomer.m_scale_y + 0.5);
	}

	// 反缩放RECT
	static void unzoom (RECT &o) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return;
		o.left = (decltype (o.left)) (o.left / zoomer.m_scale_x + 0.5);
		o.top = (decltype (o.top)) (o.top / zoomer.m_scale_y + 0.5);
		o.right = (decltype (o.right)) (o.right / zoomer.m_scale_x + 0.5);
		o.bottom = (decltype (o.bottom)) (o.bottom / zoomer.m_scale_y + 0.5);
	}

	// 反缩放Bitmap
	static void unzoom (Gdiplus::Bitmap **pbmp) {
		tool_Zoomer &zoomer = get_zoomer ();
		if (!zoomer.m_is_scale) return;
		int src_width = (*pbmp)->GetWidth ();
		int src_height = (*pbmp)->GetHeight ();
		int dest_width = src_width / zoomer.m_scale_x;
		int dest_height = src_height / zoomer.m_scale_y;
		Gdiplus::Bitmap *tmp_bmp = new Gdiplus::Bitmap (dest_width, dest_height, (*pbmp)->GetPixelFormat ());
		Gdiplus::Graphics g (tmp_bmp);
		g.DrawImage (*pbmp, Gdiplus::Rect (0, 0, dest_width, dest_height), 0, 0, src_width, src_height, Gdiplus::UnitPixel);
		delete *pbmp;
		*pbmp = tmp_bmp;
	}

private:
	static tool_Zoomer &get_zoomer () {
		static tool_Zoomer zoomer;
		return zoomer;
	}
	tool_Zoomer () {
		HDC hDC = ::GetDC (NULL);
		m_desp_x = ::GetDeviceCaps (hDC, HORZRES);
		m_desp_y = ::GetDeviceCaps (hDC, VERTRES);
		m_real_x = ::GetDeviceCaps (hDC, DESKTOPHORZRES);
		m_real_y = ::GetDeviceCaps (hDC, DESKTOPVERTRES);
		m_scale_x = m_real_x / (double) m_desp_x;
		m_scale_y = m_real_y / (double) m_desp_y;
		::ReleaseDC (NULL, hDC);
		m_is_scale = (::abs (m_scale_x - 1) > 0.0001);
	}
	bool m_is_scale = false;
	int m_desp_x = 0, m_desp_y = 0, m_real_x = 0, m_real_y = 0;
	double m_scale_x = 1.0, m_scale_y = 1.0;
};

#endif //__TOOL_ZOOMER_HPP__
