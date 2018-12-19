#ifndef __TOOL_GDIP_HPP__
#define __TOOL_GDIP_HPP__

#include <vector>
#include <Windows.h>
#include <GdiPlus.h>



class tool_Gdip {
	static bool gdip_get_encoder_clsid (LPCWSTR format, CLSID* pClsid) {
		UINT num = 0, size = 0;
		Gdiplus::GetImageEncodersSize (&num, &size);
		if (size == 0)
			return false;
		Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc (size));
		Gdiplus::GetImageEncoders (num, size, pImageCodecInfo);
		for (UINT i = 0; i < num; ++i) {
			if (_wcsicmp (pImageCodecInfo[i].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[i].Clsid;
				free (pImageCodecInfo);
				return true;
			}
		}
		free (pImageCodecInfo);
		return false;
	}

public:
	static bool gdip_save (Gdiplus::Image *img, LPCWSTR lpFile) {
		std::wstring sFile = lpFile;
		size_t p = sFile.rfind (L'.');
		if (-1 == p)
			return false;
		std::wstring sExt = &sFile[p];
		LPCWSTR lpFormat = nullptr;
		if (sExt == L".jpg")		lpFormat = L"image/jpeg";
		else if (sExt == L".png")	lpFormat = L"image/png";
		else if (sExt == L".bmp")	lpFormat = L"image/bmp";
		else if (sExt == L".gif")	lpFormat = L"image/gif";
		else							return false;

		CLSID clsid;
		if (!gdip_get_encoder_clsid (lpFormat, &clsid))
			return false;
		return (Gdiplus::Status::Ok == img->Save (lpFile, &clsid));
	}

	static bool gdip_save_animation (std::vector<Gdiplus::Image*> &vimg, LPCWSTR lpFile, size_t delay = 50) {
		size_t szImg = vimg.size (), i;
		if (0 == szImg)
			return false;
		else if (1 == szImg)
			return gdip_save (vimg[0], lpFile);

		int width = vimg[0]->GetWidth (), height = vimg[0]->GetHeight ();
		Gdiplus::Image *img = vimg[0]->Clone ();

		Gdiplus::PropertyItem propertyItem;
		short sRepeatNum = 0;
		propertyItem.id = PropertyTagLoopCount;
		propertyItem.length = sizeof (short);
		propertyItem.type = PropertyTagTypeShort;
		propertyItem.value = &sRepeatNum;
		img->SetPropertyItem (&propertyItem);
		ULONG *plValue = new ULONG[szImg];
		size_t delayVal = (size_t) (delay / 10.0 + 0.5);
		for (i = 0; i < szImg; ++i)
			plValue[i] = (ULONG) delayVal;
		propertyItem.id = PropertyTagFrameDelay;
		propertyItem.length = sizeof (ULONG) * (ULONG) szImg;
		propertyItem.type = PropertyTagTypeLong;
		propertyItem.value = plValue;
		img->SetPropertyItem (&propertyItem);
		delete[] plValue;

		CLSID clsid;
		Gdiplus::EncoderParameters encoderParams;
		gdip_get_encoder_clsid (L"image/gif", &clsid);
		encoderParams.Count = 1;
		encoderParams.Parameter[0].Guid = Gdiplus::EncoderSaveFlag;
		encoderParams.Parameter[0].NumberOfValues = 1;
		encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		long firstValue = Gdiplus::EncoderValueMultiFrame;
		encoderParams.Parameter[0].Value = &firstValue;
		img->Save (lpFile, &clsid, &encoderParams);
		firstValue = Gdiplus::EncoderValueFrameDimensionTime;
		encoderParams.Parameter[0].Value = &firstValue;
		for (i = 1; i < szImg; ++i)
			img->SaveAdd (vimg[i], &encoderParams);
		delete img;
		return true;
	}

	static Gdiplus::Bitmap* capture_screen () {
		HDC hDC = ::GetDC (NULL);
		int cx = ::GetDeviceCaps (hDC, DESKTOPHORZRES);
		int cy = ::GetDeviceCaps (hDC, DESKTOPVERTRES);
		::ReleaseDC (NULL, hDC);
		Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap (cx, cy, PixelFormat24bppRGB);
		Gdiplus::Graphics g (bmp);
		HDC hDeskDC = ::GetDC (NULL);
		HDC hGDC = g.GetHDC ();
		::BitBlt (hGDC, 0, 0, cx, cy, hDeskDC, 0, 0, SRCCOPY);
		g.ReleaseHDC (hGDC);
		::ReleaseDC (NULL, hDeskDC);
		return bmp;
	}

	static HBITMAP capture_screen_gdi () {
		HDC hDeskDC = ::GetDC (NULL);// ::GetWindowDC (NULL);
		HDC hMemDc = ::CreateCompatibleDC (hDeskDC);
		int cx = ::GetSystemMetrics (SM_CXSCREEN);
		int cy = ::GetSystemMetrics (SM_CYSCREEN);
		BITMAPINFO bi { { sizeof (BITMAPINFOHEADER), (LONG) cx, (LONG) cy, 1, 24, BI_RGB, (DWORD) cx * cy * 3, 5000, 5000, 0, 0 }, { 0 } };
		void *ptr = nullptr;
		HBITMAP hBmp = ::CreateDIBSection (hMemDc, &bi, DIB_RGB_COLORS, &ptr, NULL, 0);
		//HBITMAP hBmp = ::CreateCompatibleBitmap (hMemDc, cx, cy);
		::SelectObject (hMemDc, hBmp);
		::BitBlt (hMemDc, 0, 0, cx, cy, hDeskDC, 0, 0, SRCCOPY);
		::DeleteDC (hMemDc);
		::ReleaseDC (NULL, hDeskDC);
		return hBmp;
	}

	// 获取鼠标指针图片转位图
	static Gdiplus::Bitmap *cursor_to_bmp (HCURSOR hcur) {
		//CURSORINFO cinfo { sizeof (CURSORINFO) };
		//::GetCursorInfo (&cinfo);
		//HICON hIcon = cinfo.hCursor;
		ICONINFO iinfo;
		if (!::GetIconInfo ((HICON) hcur, &iinfo))
			return nullptr;
		Gdiplus::Bitmap *b = Gdiplus::Bitmap::FromHBITMAP (iinfo.hbmColor, NULL);
		//Gdiplus::Bitmap *bm = Gdiplus::Bitmap::FromHBITMAP (iinfo.hbmMask, NULL);
		Gdiplus::Bitmap *bs = new Gdiplus::Bitmap (b->GetWidth (), b->GetHeight (), PixelFormat32bppARGB);
		Gdiplus::Graphics g (bs);
		HDC hDC = g.GetHDC ();
		::DrawIcon (hDC, 0, 0, (HICON) hcur);
		g.ReleaseHDC (hDC);
		//Gdiplus::Color c (0);
		//for (UINT i = 0; i < b->GetWidth (); ++i) {
		//	for (UINT j = 0; j < b->GetHeight (); ++j) {
		//		bm->GetPixel (i, j, &c);
		//		Gdiplus::ARGB argb = c.GetValue ();
		//		if (!(c.GetValue () & 0xffffff)) {
		//			b->GetPixel (i, j, &c);
		//			bs->SetPixel (i, j, c);
		//		}
		//	}
		//}
		//delete bm;
		delete b;
		return bs;
	}
};

#endif //__TOOL_GDIP_HPP__
