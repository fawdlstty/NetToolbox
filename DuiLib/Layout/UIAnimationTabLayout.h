#ifndef __UIANIMATIONTABLAYOUT_H__
#define __UIANIMATIONTABLAYOUT_H__

namespace DuiLib {
	class UILIB_API CAnimationTabLayoutUI: public CTabLayoutUI, public CUIAnimation {
		DECLARE_DUICONTROL (CAnimationTabLayoutUI)
	public:
		CAnimationTabLayoutUI ();

		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);

		bool SelectItem (int iIndex);
		void AnimationSwitch ();
		void DoEvent (TEventUI& event);
		void OnTimer (int nTimerID);

		virtual void OnAnimationStart (INT nAnimationID, BOOL bFirstLoop) {}
		virtual void OnAnimationStep (INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		virtual void OnAnimationStop (INT nAnimationID);

		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

	protected:
		bool		m_bIsVerticalDirection	= false;
		int			m_nPositiveDirection	= 1;
		RECT		m_rcCurPos				= { 0 };
		RECT		m_rcItemOld				= { 0 };
		CControlUI	*m_pCurrentControl		= nullptr;
		bool		m_bControlVisibleFlag	= false;
		enum {
			TAB_ANIMATION_ID = 1,

			TAB_ANIMATION_ELLAPSE = 10,
			TAB_ANIMATION_FRAME_COUNT = 15,
		};
	};
}
#endif // __UIANIMATIONTABLAYOUT_H__