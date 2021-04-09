#ifndef __PAGE_RECORD_HPP__
#define __PAGE_RECORD_HPP__

#include <thread>
#include <chrono>

#include <Windows.h>
#include <tchar.h>

//#include <libavcodec/avcodec.h>

//#include "../tools/tool_MakeAvi.hpp"
//#include "../tools/tool_VP9.hpp"
#include "../tools/tool_Gdip.hpp"

#include "page_base.hpp"



class page_Record: public page_base {
public:
	page_Record (NetToolboxWnd *parent): page_base (parent) {
		faw::string_t path = faw::Directory::get_current_path ().str ();
		m_record_path->SetText (fmt::format (_T ("%c:\\record.gif"), path[0]));
	}
	virtual ~page_Record () = default;

	bool OnClick (TNotifyUI& msg) override {
		 faw::string_t name = msg.pSender->GetName ();
		if (name == _T ("record_begin")) {
			//tool_VP9 (_T ("D:/a.ivf"));
			//
			//tool_MakeAvi avi (_T ("D:/a.avi"));
			//HBITMAP hbmp = tool_Gdip::capture_screen_gdi ();
			//bool bRet = avi.init (hbmp);
			//::DeleteObject (hbmp);
			//if (bRet) {
			//	auto tp = std::chrono::system_clock::now ();
			//	for (size_t i = 0; i < 100; ++i) {
			//		hbmp = tool_Gdip::capture_screen_gdi ();
			//		bRet = avi.add_frame (hbmp);
			//		::DeleteObject (hbmp);
			//		if (!bRet) {
			//			DWORD d = ::GetLastError ();
			//			d = d;
			//			break;
			//		}
			//		tp += std::chrono::milliseconds (1000 / 30);
			//		std::this_thread::sleep_until (tp);
			//	}
			//} else {
			//	DWORD d = ::GetLastError ();
			//	d = d;
			//}
			return true;
		}
		return false;
	}

protected:
	BindEditUI						m_record_path { _T ("record_path") };
};

#endif //__PAGE_RECORD_HPP__
