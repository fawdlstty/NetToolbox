////////////////////////////////////////////////////////////////////////////////
//
//  CHanAnimUI
//      自定义控件，用于实现动态效果功能
//
//  用法示例（模拟Container）：
//      <hanAnim name="img_status" width="16" height="16" bkimage="file='status_small.png' source='0,0,16,16'" />
//
//  主窗口继承自IDialogBuilderCallback，然后实现函数：
//      CControlUI* CreateControl (LPCTSTR pstrClass) override {
//          if (_tcsicmp (pstrClass, _T ("hanAnim")) == 0) return CHanAnimUI::CreateControl ();
//          return WindowImplBase::CreateControl (pstrClass);
//	    }
//
////////////////////////////////////////////////////////////////////////////////



#ifndef __HAN_ANIM_HPP__
#define __HAN_ANIM_HPP__

namespace DuiLib {
	class CHanAnimUI: public CControlUI {
	public:
		CHanAnimUI (): CControlUI (), m_imglist () {}
		virtual ~CHanAnimUI () = default;

		static CControlUI* CreateControl () {
			return new CHanAnimUI ();
		}
		faw::string_t GetClass () const {
			return _T ("CHanAnimUI");
		}
		LPVOID GetInterface (faw::string_t pstrName) override {
			if (pstrName == _T ("HanAnim")) return static_cast<CHanAnimUI*>(this);
			return CControlUI::GetInterface (pstrName);
		}

		virtual void SetBkImage (faw::string_t pStrImage) {
			if (m_is_animate) {
				this->KillTimer (m_timer_id);
				m_is_animate = FALSE;
			}
			CControlUI::SetBkImage (pStrImage);
		}

		void SetAnimateBkImage (faw::string_t pStrImage, int width, int height, int framecount = 0, int speed = 100) {
			if (m_is_animate) this->KillTimer (m_timer_id);
			BOOL _hori = width > height;
			int _width, _height; _width = _height = (width > height ? height : width);
			if (!framecount) framecount = _hori ? (width / height) : (height / width);
			if (m_imglist.size ()) m_imglist.clear ();
			faw::string_t _buf = _T ("");
			for (int i = 0; i < framecount; ++i) {
				if (_hori) {
					_buf = fmt::format (_T ("file='{}' source='{},{},{},{}'"), pStrImage, _width * i, 0, _width * (i + 1), _height);
				} else {
					_buf = fmt::format (_T ("file='{}' source='{},{},{},{}'"), pStrImage, 0, _height * i, _width, _height * (i + 1));
				}
				if (this->GetWidth () != _width || this->GetHeight () != _height) {
					_buf += fmt::format (_T (" dest='{},{},{},{}'"), this->GetWidth () > _width ? this->GetWidth () - _width : 0, 0, this->GetWidth (), _height);
				}
				m_imglist.push_back (_buf);
			}
			m_moment_frame = -1;
			m_is_animate = true;
			this->SetTimer (m_timer_id, speed);
		}

		void DoEvent (TEventUI& event) override {
			if (event.Type == UIEVENT_TIMER && event.wParam == m_timer_id) {
				CControlUI::SetBkImage (m_imglist[m_moment_frame = ++m_moment_frame % m_imglist.size ()]);
			} else {
				return CControlUI::DoEvent (event);
			}
		}

	private:
		int							m_moment_frame = -1;
		bool						m_is_animate = false;
		std::vector<faw::string_t>	m_imglist;
		UINT						m_timer_id = (UINT) this;

	public:
		static LPCTSTR			s_interface;
	};
}

#ifdef DEF_BINDCTRL
DEF_BINDCTRL (HanAnim);
#endif //DEF_BINDCTRL

#endif //__HAN_ANIM_HPP__
