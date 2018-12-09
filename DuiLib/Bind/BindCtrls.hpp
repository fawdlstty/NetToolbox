#ifndef __BIND_CTRLS_HPP__
#define __BIND_CTRLS_HPP__

#pragma once

namespace DuiLib {
#define DEF_BINDCTRL(CTRL_TYPE)																									\
	class Bind##CTRL_TYPE##UI: public BindCtrlBase {																			\
	public:																														\
		Bind##CTRL_TYPE##UI (string_view_t ctrl_name, CPaintManagerUI *pm = nullptr): BindCtrlBase (ctrl_name), m_pm (pm) {}	\
		C##CTRL_TYPE##UI *operator* () noexcept {																				\
			if (!m_ctrl) {																										\
				if (!m_pm) {																									\
					if (!(m_pm = CPaintManagerUI::GetPaintManager (_T (""))))													\
						ASSERT (false);																							\
				}																												\
				m_ctrl = m_pm->FindControl (m_ctrl_name);																		\
			}																													\
			return static_cast<C##CTRL_TYPE##UI*> (m_ctrl);																		\
		}																														\
		C##CTRL_TYPE##UI *operator-> () noexcept { return operator* (); }														\
	protected:																													\
		string_view_t GetClassType () const override { return _T (#CTRL_TYPE##"UI"); }											\
		CPaintManagerUI *m_pm = nullptr;																						\
	}



	// Core
	DEF_BINDCTRL (Control);
	//class BindControlUI: public BindCtrlBase {
	//public:
	//	BindControlUI (string_view_t ctrl_name, CPaintManagerUI *pm = nullptr): BindCtrlBase (ctrl_name), m_pm (pm) {}
	//	CControlUI *operator* () noexcept {
	//		if (!m_ctrl) {
	//			if (!m_pm) {
	//				if (!(m_pm = CPaintManagerUI::GetPaintManager (_T (""))))
	//					ASSERT (false);
	//			}
	//			m_ctrl = m_pm->FindControl (m_ctrl_name);
	//		}
	//		return static_cast<CControlUI*> (m_ctrl);
	//	}
	//	CControlUI *operator-> () noexcept { return operator* (); }
	//protected:
	//	string_view_t GetClassType () const override { return _T ("ControlUI"); }
	//	CPaintManagerUI *m_pm = nullptr;
	//};
	DEF_BINDCTRL (Container);

	// Control
	DEF_BINDCTRL (ActiveX);
	//DEF_BINDCTRL (Animation);
	DEF_BINDCTRL (Button);
	DEF_BINDCTRL (ColorPalette);
	DEF_BINDCTRL (Combo);
	DEF_BINDCTRL (ComboBox);
	DEF_BINDCTRL (DateTime);
	DEF_BINDCTRL (Edit);
	DEF_BINDCTRL (FadeButton);
	DEF_BINDCTRL (Flash);
	DEF_BINDCTRL (GifAnim);
#ifdef USE_XIMAGE_EFFECT
	DEF_BINDCTRL (GifAnimEx);
#endif
	DEF_BINDCTRL (GroupBox);
	DEF_BINDCTRL (HotKey);
	DEF_BINDCTRL (IPAddress);
	DEF_BINDCTRL (IPAddressEx);
	DEF_BINDCTRL (Label);
	DEF_BINDCTRL (List);
	DEF_BINDCTRL (ListEx);
	DEF_BINDCTRL (Menu);
	DEF_BINDCTRL (Option);
	DEF_BINDCTRL (CheckBox);
	DEF_BINDCTRL (Progress);
	DEF_BINDCTRL (RichEdit);
	DEF_BINDCTRL (Ring);
	DEF_BINDCTRL (RollText);
	DEF_BINDCTRL (ScrollBar);
	DEF_BINDCTRL (Slider);
	DEF_BINDCTRL (Text);
	DEF_BINDCTRL (TreeView);
	DEF_BINDCTRL (WebBrowser);

	// Layout
	DEF_BINDCTRL (AnimationTabLayout);
	DEF_BINDCTRL (ChildLayout);
	DEF_BINDCTRL (HorizontalLayout);
	DEF_BINDCTRL (TabLayout);
	DEF_BINDCTRL (TileLayout);
	DEF_BINDCTRL (VerticalLayout);
}

#endif //__BIND_CTRLS_HPP__
