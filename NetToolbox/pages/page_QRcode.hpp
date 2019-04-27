#ifndef __PAGE_QRCODE_HPP__
#define __PAGE_QRCODE_HPP__

#include <vector>

#include <Windows.h>
#include <tchar.h>

#include "../tools/tool_Gdip.hpp"
extern "C" {
#include "../../QRCode/qrcode.h"
}
#ifdef _DEBUG
#pragma comment (lib, "../lib/QRCode_d.lib")
#else
#pragma comment (lib, "../lib/QRCode.lib")
#endif

#include "page_base.hpp"



class page_QRcode: public page_base {
public:
	page_QRcode (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_QRcode () {
		if (m_bmp) {
			delete m_bmp;
			m_bmp = nullptr;
		}
	}

	bool OnClick (TNotifyUI& msg) override {
		faw::String name = msg.pSender->GetName ();
		if (name == _T ("qrcode_generate")) {
			std::string _str = m_qrcode_content->GetText ().stra ();
			faw::String _str_bkimg = m_qrcode_bkimg->GetText ();

			// 生成数据
			QRCode qrcode;
			uint8_t _ver = 1, *_data = nullptr;
			for (; _ver <= 40; ++_ver) {
				if (_data)
					delete[] _data;
				_data = new uint8_t[qrcode_getBufferSize (_ver)];
				bool _succ = qrcode_initText (&qrcode, _data, _ver, ECC_MEDIUM, _str.c_str ()) >= 0;
				if (_succ)
					break;
			}
			if (_ver > 40) {
				::MessageBox (m_parent->GetHWND (), International::translate (_T ("Content is too long, the generation of two-dimensional code failed")).data (), International::translate (_T ("Info")).data (), MB_ICONHAND);
				delete[] _data;
				return true;
			}

			// 生成图片
			if (m_bmp)
				delete m_bmp;
			m_bmp = new Gdiplus::Bitmap (qrcode.size * 5, qrcode.size * 5, PixelFormat32bppARGB);
			Gdiplus::Graphics g (m_bmp);
			Gdiplus::Brush *_bbrush = new Gdiplus::SolidBrush (Gdiplus::Color::MakeARGB (255, 0, 0, 0));
			Gdiplus::Brush *_wbrush = new Gdiplus::SolidBrush (Gdiplus::Color::MakeARGB (255, 255, 255, 255));
			for (size_t y = 0; y < (size_t) qrcode.size; ++y) {
				for (size_t x = 0; x < (size_t) qrcode.size; ++x) {
					g.FillRectangle (qrcode_getModule (&qrcode, (uint8_t) x, (uint8_t) y) ? _bbrush : _wbrush, Gdiplus::Rect (x * 5, y * 5, 5, 5));
				}
			}
			//tool_Gdip::gdip_save (m_bmp, L"d:/text.png");
			delete _wbrush;
			delete _bbrush;

			delete[] _data;
			return true;
		}
		return false;
	}

protected:
	BindEditUI						m_qrcode_content { _T ("qrcode_content") };
	BindEditUI						m_qrcode_bkimg { _T ("qrcode_bkimg") };

	Gdiplus::Bitmap					*m_bmp = nullptr;
};

#endif //__PAGE_QRCODE_HPP__
