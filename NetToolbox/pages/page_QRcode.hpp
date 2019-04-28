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
			//// begin 测试
			//for (size_t _i = 20; _i <= 20; ++_i) {
			//	uint8_t *_data = new uint8_t[qrcode_getBufferSize (_i)];
			//	qrcode_initText (&qrcode, _data, _i, ECC_MEDIUM, _str.c_str ());
			//	faw::String _path = faw::String::format (_T ("D:/__img__test_/_%d_%d.png"), _i, qrcode.size);
			//	if (m_bmp)
			//		delete m_bmp;
			//	m_bmp = new Gdiplus::Bitmap (qrcode.size * 5, qrcode.size * 5, PixelFormat32bppARGB);
			//	Gdiplus::Graphics g (m_bmp);
			//	Gdiplus::Color _bcolor = Gdiplus::Color::MakeARGB (255, 0, 0, 0);
			//	Gdiplus::Color _wcolor = Gdiplus::Color::MakeARGB (255, 255, 255, 255);
			//	Gdiplus::Brush *_bbrush = new Gdiplus::SolidBrush (_bcolor);
			//	Gdiplus::Brush *_wbrush = new Gdiplus::SolidBrush (_wcolor);
			//	for (size_t y = 0; y < (size_t) qrcode.size; ++y) {
			//		for (size_t x = 0; x < (size_t) qrcode.size; ++x) {
			//			bool _is_black = qrcode_getModule (&qrcode, (uint8_t) x, (uint8_t) y);
			//			bool _identfer = _is_identfer (_i, x, y);
			//			if (_identfer) {
			//				g.FillRectangle (_is_black ? _bbrush : _wbrush, Gdiplus::Rect (x * 5, y * 5, 5, 5));
			//			} else {
			//				m_bmp->SetPixel (x * 5 + 2, y * 5 + 2, _is_black ? _bcolor : _wcolor);
			//			}
			//		}
			//	}
			//	tool_Gdip::gdip_save (m_bmp, _path.strw ().c_str ());
			//	delete[] _data;
			//}
			//// end 测试
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
					_is_identfer (_ver, x, y);
				}
			}
			//tool_Gdip::gdip_save (m_bmp, L"d:/text.png");
			delete _wbrush;
			delete _bbrush;

			delete[] _data;
			return true;
		} else if (name == _T ("qrcode_save")) {

		}
		return false;
	}

protected:
	static bool _is_identfer (uint8_t _ver, int _x, int _y) {
		auto _calc_near = [_x, _y] (int _dst_x, int _dst_y) {
			return (::abs (_dst_x - _x) < 3 && ::abs (_dst_y - _y) < 3);
		};
		int _r_len = 17 + _ver * 4 - 1;
		int _r_len_s = _r_len / 2;
		auto _calc_4near = [_x, _y, _r_len, _calc_near] (int _mid1, int _mid2) {
			return _calc_near (_mid1, 6) || _calc_near (_mid2, 6) || _calc_near (6, _mid1) || _calc_near (_mid1, _mid1) || _calc_near (_mid2, _mid1) || _calc_near (_r_len - 6, _mid1) || _calc_near (6, _mid2) || _calc_near (_mid1, _mid2) || _calc_near (_mid2, _mid2) || _calc_near (_r_len - 6, _mid2) || _calc_near (_mid1, _r_len - 6) || _calc_near (_mid2, _r_len - 6);
		};

		if (_x == 6 || _y == 6 || (_x <= 7 && _y <= 7) || (_x <= 7 && _y >= _r_len - 7) || (_x >= _r_len - 7 && _y <= 7)) {
			return true;
		}
		if (_ver == 1) {
			return false;
		}
		if (_calc_near (_r_len - 6, _r_len - 6))
			return true;
		if (_ver >= 2 && _ver <= 6) {
			return false;
		} else if (_ver >= 7 && _ver <= 13) {
			int _sx = _r_len_s - (::abs (_x - _r_len_s));
			int _sy = _r_len_s - (::abs (_y - _r_len_s));
			auto _calc_snear = [_sx, _sy] (int _dst_x, int _dst_y) {
				return (::abs (_dst_x - _sx) < 3 && ::abs (_dst_y - _sy) < 3);
			};
			return _calc_near (_r_len_s, _r_len_s) || _calc_snear (_r_len_s, 6) || _calc_snear (6, _r_len_s);
		} else if (_ver == 14) {
			return _calc_4near (26, 46);
		} else if (_ver == 15) {
			return _calc_4near (26, 48);
		} else if (_ver == 16) {
			return _calc_4near (26, 50);
		} else if (_ver == 17) {
			return _calc_4near (30, 54);
		} else if (_ver == 18) {
			return _calc_4near (30, 56);
		} else if (_ver == 19) {
			return _calc_4near (30, 58);
		} else if (_ver == 20) {
			return _calc_4near (34, 62);
		}
		return false;
	}

	BindEditUI						m_qrcode_content { _T ("qrcode_content") };
	BindEditUI						m_qrcode_bkimg { _T ("qrcode_bkimg") };

	Gdiplus::Bitmap					*m_bmp = nullptr;
};

#endif //__PAGE_QRCODE_HPP__
