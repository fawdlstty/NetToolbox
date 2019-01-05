////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_MakeAvi
// Description: AVI工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Jan 05, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_MAKEAVI_HPP__
#define __TOOL_MAKEAVI_HPP__

//http://blog.sina.com.cn/s/blog_a2e5bcda01019bcw.html

#include <Vfw.h>
#pragma comment (lib, "Vfw32.lib")

#include "tool_Encoding.hpp"




class tool_MakeAvi {
public:
	tool_MakeAvi (string_view_t path): m_path (path) {}

	bool init (HBITMAP hBmp) {
		::AVIFileInit ();
		if (::AVIFileOpen (&m_pfile, m_path.c_str (), OF_CREATE | OF_WRITE, nullptr) != AVIERR_OK)
			return false;
		::GetObject (hBmp, sizeof (BITMAPINFOHEADER), &m_bih);
		m_bih.biSize = sizeof (BITMAPINFOHEADER);
		m_bih.biPlanes = 1;
		m_bih.biCompression = BI_RGB;
		m_bih.biBitCount = 24;
		m_bih.biXPelsPerMeter = m_bih.biYPelsPerMeter = 3780;
		m_bih.biSizeImage = m_bih.biWidth * m_bih.biHeight * m_bih.biBitCount / 8;
		m_buf = new unsigned char[m_bih.biSizeImage];
		//
		m_asi.fccType = streamtypeVIDEO;
		//m_asi.fccType = ICTYPE_VIDEO;
		//m_asi.fccHandler = mmioFOURCC ('D', 'I', 'B', ' ');
		//m_asi.fccHandler = mmioFOURCC ('x', 'v', 'i', 'd');
		//m_asi.fccHandler = mmioFOURCC ('i', 'v', '4', '1');
		//m_asi.fccHandler = mmioFOURCC ('M', 'P', 'G', '4');
		//m_asi.fccHandler = mmioFOURCC ('X', '2', '6', '4');
		//m_asi.fccHandler = mmioFOURCC ('M', 'P', '4', '2');
		m_asi.fccHandler = mmioFOURCC ('M', 'S', 'V', 'C');
		m_asi.dwRate = 30;
		m_asi.dwScale = 1;
		m_asi.dwQuality = -1;
		m_asi.dwSuggestedBufferSize = m_bih.biSizeImage;
		m_asi.rcFrame = { 0, 0, m_bih.biWidth, m_bih.biHeight };
		lstrcpy (m_asi.szName, _T ("NT Video Stream"));
		if (::AVIFileCreateStream (m_pfile, &m_pstm, &m_asi) != AVIERR_OK) {
			DWORD d = ::GetLastError ();
			return false;
		}
		//
		AVICOMPRESSOPTIONS aco { 0 };
		aco.fccType = m_asi.fccType;
		aco.fccHandler = m_asi.fccHandler;
		aco.dwQuality = 10000;
		aco.dwKeyFrameEvery = 30;
		aco.dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;
		aco.dwBytesPerSecond = 1000 / 30;
		if (::AVIMakeCompressedStream (&m_pstm_cprs, m_pstm, &aco, nullptr) != AVIERR_OK) {
			DWORD d = ::GetLastError ();
			return false;
		}
		if (::AVIStreamSetFormat (m_pstm_cprs, 0, &m_bih, m_bih.biSize) != AVIERR_OK) {
			DWORD d = ::GetLastError ();
			return false;
		}
		return true;
	}

	bool add_frame (HBITMAP hBmp) {
		HDC hTmpDC = ::CreateCompatibleDC (NULL);
		BITMAPINFO bi { { sizeof (BITMAPINFOHEADER) }, { 0 } };
		::GetDIBits (hTmpDC, hBmp, 0, 0, nullptr, &bi, DIB_RGB_COLORS);
		bi.bmiHeader = m_bih;
		::GetDIBits (hTmpDC, hBmp, 0, bi.bmiHeader.biHeight, m_buf, &bi, DIB_RGB_COLORS);
		::DeleteDC (hTmpDC);
		return ::AVIStreamWrite (m_pstm_cprs, m_next_frame++, 1, m_buf, sizeof (m_buf), AVICOMPRESSF_KEYFRAMES, nullptr, nullptr) == AVIERR_OK;
	}

	virtual ~tool_MakeAvi () {
		if (m_buf)
			delete[] m_buf;
		if (m_astm)
			::AVIStreamRelease (m_astm);
		if (m_pstm_cprs)
			::AVIStreamRelease (m_pstm_cprs);
		if (m_pstm)
			::AVIStreamRelease (m_pstm);
		if (m_pfile)
			::AVIFileRelease (m_pfile);
		::AVIFileExit ();
	}

private:
	BITMAPINFOHEADER	m_bih { sizeof (BITMAPINFOHEADER) };
	AVISTREAMINFO		m_asi { 0 };
	string_t			m_path = _T ("");
	IAVIFile			*m_pfile = nullptr;
	//WAVEFORMATEX		m_wfx { 0 };
	int					m_period = 50;						// 帧时间间隔
	IAVIStream			*m_astm = nullptr;
	IAVIStream			*m_pstm = nullptr;
	IAVIStream			*m_pstm_cprs = nullptr;
	unsigned long		m_next_frame = 0;
	unsigned char		*m_buf = nullptr;
};



#endif //__TOOL_MAKEAVI_HPP__
