#ifndef __PAGE_QRCODE_HPP__
#define __PAGE_QRCODE_HPP__

#include <vector>

#include <Windows.h>
#include <tchar.h>

#include "../tools/tool_Gdip.hpp"
#include "../3rdparty/QrCode/QrCode.hpp"

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
		faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("qrcode_generate")) {
			std::string _str = faw::Encoding::T_to_utf8 (m_qrcode_content->GetText ());
			faw::string_t _str_bkimg = m_qrcode_bkimg->GetText ();

			// 生成数据
			const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText (_str.data (), qrcodegen::QrCode::Ecc::LOW);
			//QRCode qrcode;
			//uint8_t _ver = 1, *_data = nullptr;
			//for (; _ver <= 40; ++_ver) {
			//	if (_data)
			//		delete[] _data;
			//	_data = new uint8_t[qrcode_getBufferSize (_ver)];
			//	bool _succ = qrcode_initText (&qrcode, _data, _ver, ECC_MEDIUM, _str.data ()) >= 0;
			//	if (_succ)
			//		break;
			//}
			//if (_ver > 40) {
			//	::MessageBox (m_parent->GetHWND (), _IT (_T ("Content is too long, the generation of two-dimensional code failed")).data (), _IT (_T ("Info")).data (), MB_ICONHAND);
			//	delete[] _data;
			//	return true;
			//}

			// 生成图片
			if (m_bmp)
				delete m_bmp;
			m_bmp = new Gdiplus::Bitmap (qr.getSize () * 5, qr.getSize () * 5, PixelFormat32bppARGB);
			Gdiplus::Graphics g (m_bmp);
			Gdiplus::Brush *_bbrush = new Gdiplus::SolidBrush (Gdiplus::Color::MakeARGB (255, 0, 0, 0));
			Gdiplus::Brush *_wbrush = new Gdiplus::SolidBrush (Gdiplus::Color::MakeARGB (255, 255, 255, 255));
			for (size_t y = 0; y < (size_t) qr.getSize (); ++y) {
				for (size_t x = 0; x < (size_t) qr.getSize (); ++x) {
					g.FillRectangle (qr.getModule (x, y) ? _bbrush : _wbrush, Gdiplus::Rect (x * 5, y * 5, 5, 5));
					_is_identfer (qr.getVersion (), x, y);
				}
			}
			//tool_Gdip::gdip_save (m_bmp, L"d:/text.png");
			delete _wbrush;
			delete _bbrush;

			//delete[] _data;
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
