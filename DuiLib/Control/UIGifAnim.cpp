#include "StdAfx.h"
#include "UIGifAnim.h"

///////////////////////////////////////////////////////////////////////////////////////
//DECLARE_HANDLE (HZIP);	// An HZIP identifies a zip file that has been opened
//typedef DWORD ZRESULT;
//typedef struct {
//	int index;                 // index of this file within the zip
//	char name[MAX_PATH];       // filename within the zip
//	DWORD attr;                // attributes, as in GetFileAttributes.
//	FILETIME atime, ctime, mtime;// access, create, modify filetimes
//	long comp_size;            // sizes of item, compressed and uncompressed. These
//	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
//} ZIPENTRY;
//typedef struct {
//	int index;                 // index of this file within the zip
//	TCHAR name[MAX_PATH];      // filename within the zip
//	DWORD attr;                // attributes, as in GetFileAttributes.
//	FILETIME atime, ctime, mtime;// access, create, modify filetimes
//	long comp_size;            // sizes of item, compressed and uncompressed. These
//	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
//} ZIPENTRYW;
//#define OpenZip OpenZipU
//#define CloseZip(hz) CloseZipU(hz)
//extern HZIP OpenZipU (void *z, unsigned int len, DWORD flags);
//extern ZRESULT CloseZipU (HZIP hz);
//#ifdef _UNICODE
//#define ZIPENTRY ZIPENTRYW
//#define GetZipItem GetZipItemW
//#define FindZipItem FindZipItemW
//#else
//#define GetZipItem GetZipItemA
//#define FindZipItem FindZipItemA
//#endif
//extern ZRESULT GetZipItemA (HZIP hz, int index, ZIPENTRY *ze);
//extern ZRESULT GetZipItemW (HZIP hz, int index, ZIPENTRYW *ze);
//extern ZRESULT FindZipItemA (HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
//extern ZRESULT FindZipItemW (HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
//extern ZRESULT UnzipItem (HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
namespace DuiLib {
	IMPLEMENT_DUICONTROL (CGifAnimUI)
	CGifAnimUI::CGifAnimUI (void) {}


	CGifAnimUI::~CGifAnimUI (void) {
		DeleteGif ();
		m_pManager->KillTimer (this, EVENT_TIEM_ID);

	}

	faw::string_view_t CGifAnimUI::GetClass () const {
		return _T ("GifAnimUI");
	}

	LPVOID CGifAnimUI::GetInterface (faw::string_view_t pstrName) {
		if (pstrName == DUI_CTRL_GIFANIM) return static_cast<CGifAnimUI*>(this);
		return CControlUI::GetInterface (pstrName);
	}

	void CGifAnimUI::DoInit () {
		InitGifImage ();
	}

	bool CGifAnimUI::DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
		if (!::IntersectRect (&m_rcPaint, &rcPaint, &m_rcItem)) return true;
		if (!m_pGifImage) {
			InitGifImage ();
		}
		DrawFrame (hDC);
		return true;
	}

	void CGifAnimUI::DoEvent (TEventUI& event) {
		if (event.Type == UIEVENT_TIMER)
			OnTimer ((UINT_PTR) event.wParam);
	}

	void CGifAnimUI::SetVisible (bool bVisible /* = true */) {
		CControlUI::SetVisible (bVisible);
		if (bVisible)
			PlayGif ();
		else
			StopGif ();
	}

	void CGifAnimUI::SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue) {
		if (pstrName == _T ("bkimage")) SetBkImage (pstrValue);
		else if (pstrName == _T ("autoplay")) {
			SetAutoPlay (FawTools::parse_bool (pstrValue));
		} else if (pstrName == _T ("autosize")) {
			SetAutoSize (FawTools::parse_bool (pstrValue));
		} else
			CControlUI::SetAttribute (pstrName, pstrValue);
	}

	void CGifAnimUI::SetBkImage (faw::string_view_t pStrImage) {
		if (m_sBkImage == pStrImage || nullptr == pStrImage) return;

		m_sBkImage = pStrImage;

		StopGif ();
		DeleteGif ();

		Invalidate ();

	}

	faw::string_view_t CGifAnimUI::GetBkImage () {
		return m_sBkImage.str_view ();
	}

	void CGifAnimUI::SetAutoPlay (bool bIsAuto) {
		m_bIsAutoPlay = bIsAuto;
	}

	bool CGifAnimUI::IsAutoPlay () const {
		return m_bIsAutoPlay;
	}

	void CGifAnimUI::SetAutoSize (bool bIsAuto) {
		m_bIsAutoSize = bIsAuto;
	}

	bool CGifAnimUI::IsAutoSize () const {
		return m_bIsAutoSize;
	}

	void CGifAnimUI::PlayGif () {
		if (m_bIsPlaying || !m_pGifImage || m_nFrameCount <= 1) {
			return;
		}

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if (lPause == 0) lPause = 100;
		m_pManager->SetTimer (this, EVENT_TIEM_ID, lPause);

		m_bIsPlaying = true;
	}

	void CGifAnimUI::PauseGif () {
		if (!m_bIsPlaying || !m_pGifImage) {
			return;
		}

		m_pManager->KillTimer (this, EVENT_TIEM_ID);
		this->Invalidate ();
		m_bIsPlaying = false;
	}

	void CGifAnimUI::StopGif () {
		if (!m_bIsPlaying) {
			return;
		}

		m_pManager->KillTimer (this, EVENT_TIEM_ID);
		m_nFramePosition = 0;
		this->Invalidate ();
		m_bIsPlaying = false;
	}

	void CGifAnimUI::InitGifImage () {
		m_pGifImage = CRenderEngine::GdiplusLoadImage (GetBkImage ());
		if (!m_pGifImage) return;
		UINT nCount = 0;
		nCount = m_pGifImage->GetFrameDimensionsCount ();
		GUID* pDimensionIDs = new GUID[nCount];
		m_pGifImage->GetFrameDimensionsList (pDimensionIDs, nCount);
		m_nFrameCount = m_pGifImage->GetFrameCount (&pDimensionIDs[0]);
		if (m_nFrameCount > 1) {
			int nSize = m_pGifImage->GetPropertyItemSize (PropertyTagFrameDelay);
			m_pPropertyItem = (Gdiplus::PropertyItem*) malloc (nSize);
			m_pGifImage->GetPropertyItem (PropertyTagFrameDelay, nSize, m_pPropertyItem);
		}
		delete[]  pDimensionIDs;
		pDimensionIDs = nullptr;

		if (m_bIsAutoSize) {
			SetFixedWidth (m_pGifImage->GetWidth ());
			SetFixedHeight (m_pGifImage->GetHeight ());
		}
		if (m_bIsAutoPlay) {
			PlayGif ();
		}
	}

	void CGifAnimUI::DeleteGif () {
		if (m_pStream) {
			m_pStream->Release ();
			m_pStream = nullptr;
		}
		if (m_pGifImage) {
			delete m_pGifImage;
			m_pGifImage = nullptr;
		}

		if (m_pPropertyItem) {
			free (m_pPropertyItem);
			m_pPropertyItem = nullptr;
		}
		m_nFrameCount = 0;
		m_nFramePosition = 0;
	}

	void CGifAnimUI::OnTimer (UINT_PTR idEvent) {
		if (idEvent != EVENT_TIEM_ID)
			return;
		m_pManager->KillTimer (this, EVENT_TIEM_ID);
		this->Invalidate ();

		m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if (lPause == 0) lPause = 100;
		m_pManager->SetTimer (this, EVENT_TIEM_ID, lPause);
	}

	void CGifAnimUI::DrawFrame (HDC hDC) {
		if (!hDC || !m_pGifImage) return;
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		Gdiplus::Graphics graphics (hDC);
		graphics.DrawImage (m_pGifImage, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
		m_pGifImage->SelectActiveFrame (&pageGuid, m_nFramePosition);
	}

	Gdiplus::Image* CGifAnimUI::LoadGifFromFile (faw::string_view_t pstrGifPath) {
		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do {
			faw::String sFile = CPaintManagerUI::GetResourcePath ();
			if (CPaintManagerUI::GetResourceZip ().empty ()) {
				sFile += pstrGifPath;
				HANDLE hFile = ::CreateFile (sFile.c_str (), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE) break;
				dwSize = ::GetFileSize (hFile, NULL);
				if (dwSize == 0) break;

				DWORD dwRead = 0;
				pData = new BYTE[dwSize];
				::ReadFile (hFile, pData, dwSize, &dwRead, NULL);
				::CloseHandle (hFile);

				if (dwRead != dwSize) {
					delete[] pData;
					pData = NULL;
					break;
				}
			} else {
				sFile += CPaintManagerUI::GetResourceZip ();
				HZIP hz = NULL;
				if (CPaintManagerUI::IsCachedResourceZip ()) hz = (HZIP) CPaintManagerUI::GetResourceZipHandle ();
				//else hz = OpenZip ((void*) sFile, 0, 2);
				else hz = OpenZip (sFile.c_str ());
				if (hz == NULL) break;
				ZIPENTRY ze;
				int i;
				if (FindZipItem (hz, pstrGifPath.data (), true, &i, &ze) != 0) break;
				dwSize = ze.unc_size;
				if (dwSize == 0) break;
				pData = new BYTE[dwSize];
				//int res = UnzipItem (hz, i, pData, dwSize, 3);
				int res = UnzipItem (hz, i, pData, dwSize);
				if (res != 0x00000000 && res != 0x00000600) {
					delete[] pData;
					pData = NULL;
					if (!CPaintManagerUI::IsCachedResourceZip ()) CloseZip (hz);
					break;
				}
				if (!CPaintManagerUI::IsCachedResourceZip ()) CloseZip (hz);
			}

		} while (0);

		while (!pData) {
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile (pstrGifPath.data (), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) break;
			dwSize = ::GetFileSize (hFile, NULL);
			if (dwSize == 0) break;

			DWORD dwRead = 0;
			pData = new BYTE[dwSize];
			::ReadFile (hFile, pData, dwSize, &dwRead, NULL);
			::CloseHandle (hFile);

			if (dwRead != dwSize) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}
		if (!pData) {
			return NULL;
		}

		Gdiplus::Image* pImage = LoadGifFromMemory (pData, dwSize);
		delete[] pData;
		return pImage;
	}

	Gdiplus::Image* CGifAnimUI::LoadGifFromMemory (LPVOID pBuf, size_t dwSize) {
		HGLOBAL hMem = ::GlobalAlloc (GMEM_MOVEABLE, dwSize);
		BYTE* pMem = (BYTE*)::GlobalLock (hMem);

		memcpy (pMem, pBuf, dwSize);
		::GlobalUnlock (hMem);

		::CreateStreamOnHGlobal (hMem, TRUE, &m_pStream);
		Gdiplus::Image *pImg = Gdiplus::Image::FromStream (m_pStream);
		if (!pImg || pImg->GetLastStatus () != Gdiplus::Ok) {
			m_pStream->Release ();
			m_pStream = NULL;
			return 0;
		}
		return pImg;
	}
}