#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#pragma once
#define WM_USER_SET_DPI WM_USER + 200
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CControlUI;
	class CRichEditUI;
	class CIDropTarget;

	/////////////////////////////////////////////////////////////////////////////////////
	//
	enum UILIB_RESTYPE {
		UILIB_FILE = 1,		// 来自磁盘文件
		UILIB_ZIP,			// 来自磁盘zip压缩包
		UILIB_RESOURCE,		// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
	};
	/////////////////////////////////////////////////////////////////////////////////////
	//

	enum EVENTTYPE_UI {
		UIEVENT__FIRST = 1,
		UIEVENT__KEYBEGIN,
		UIEVENT_KEYDOWN,
		UIEVENT_KEYUP,
		UIEVENT_CHAR,
		UIEVENT_SYSKEY,
		UIEVENT__KEYEND,
		UIEVENT__MOUSEBEGIN,
		UIEVENT_MOUSEMOVE,
		UIEVENT_MOUSELEAVE,
		UIEVENT_MOUSEENTER,
		UIEVENT_MOUSEHOVER,
		UIEVENT_BUTTONDOWN,
		UIEVENT_BUTTONUP,
		UIEVENT_RBUTTONDOWN,
		UIEVENT_RBUTTONUP,
		UIEVENT_MBUTTONDOWN,
		UIEVENT_MBUTTONUP,
		UIEVENT_DBLCLICK,
		UIEVENT_CONTEXTMENU,
		UIEVENT_SCROLLWHEEL,
		UIEVENT__MOUSEEND,
		UIEVENT_KILLFOCUS,
		UIEVENT_SETFOCUS,
		UIEVENT_WINDOWSIZE,
		UIEVENT_SETCURSOR,
		UIEVENT_TIMER,
		UIEVENT__LAST,
	};

	enum MSGTYPE_UI {
		// 内部保留消息
		UIMSG_TRAYICON = WM_USER + 1,
		// 程序自定义消息
		UIMSG_USER = WM_USER + 100,
	};
	/////////////////////////////////////////////////////////////////////////////////////
	//

	// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

	// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_UPDATETEST    0x00000008
#define UIFIND_TOP_FIRST     0x00000010
#define UIFIND_ME_FIRST      0x80000000

	// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



	/////////////////////////////////////////////////////////////////////////////////////
	//

	typedef struct UILIB_API tagTFontInfo {
		HFONT hFont;
		CDuiString sFontName;
		int iSize;
		bool bBold;
		bool bUnderline;
		bool bItalic;
		TEXTMETRIC tm;
	} TFontInfo;

	typedef struct UILIB_API tagTImageInfo {
		HBITMAP hBitmap			= NULL;
		HBITMAP *phBitmap		= nullptr;
		LPBYTE pBits			= nullptr;
		LPBYTE pSrcBits			= nullptr;
		int nX					= 0;
		int nY					= 0;
		bool bAlpha				= false;
		bool bUseHSL			= false;
		CDuiString sResType		= _T ("");
		DWORD dwMask			= 0;
	} TImageInfo;

	typedef struct UILIB_API tagTDrawInfo {
		tagTDrawInfo ();
		void Parse (string_view_t pStrImage, string_view_t pStrModify, CPaintManagerUI *paintManager);
		void Clear ();

		CDuiString sDrawString;
		CDuiString sDrawModify;
		CDuiString sImageName;
		CDuiString sResType;
		RECT rcDest = { 0 };
		RECT rcSource = { 0 };
		RECT rcCorner = { 0 };
		DWORD dwMask;
		BYTE uFade;
		bool bHole;
		bool bTiledX;
		bool bTiledY;
		bool bHSL;
	} TDrawInfo;

	typedef struct UILIB_API tagTPercentInfo {
		double left;
		double top;
		double right;
		double bottom;
	} TPercentInfo;

	typedef struct UILIB_API tagTResInfo {
		DWORD m_dwDefaultDisabledColor;
		DWORD m_dwDefaultFontColor;
		DWORD m_dwDefaultLinkFontColor;
		DWORD m_dwDefaultLinkHoverFontColor;
		DWORD m_dwDefaultSelectedBkColor;
		TFontInfo m_DefaultFontInfo;
		CStdStringPtrMap m_CustomFonts;
		CStdStringPtrMap m_ImageHash;
		CStdStringPtrMap m_AttrHash;
		CStdStringPtrMap m_StyleHash;
		CStdStringPtrMap m_DrawInfoHash;
	} TResInfo;

	// Structure for notifications from the system
	// to the control implementation.
	typedef struct UILIB_API tagTEventUI {
		int Type;
		CControlUI* pSender;
		DWORD dwTimestamp;
		POINT ptMouse = { 0 };
		TCHAR chKey;
		WORD wKeyState;
		WPARAM wParam;
		LPARAM lParam;
	} TEventUI;

	// Drag&Drop control
	const TCHAR* const CF_MOVECONTROL = _T ("CF_MOVECONTROL");

	typedef struct UILIB_API tagTCFMoveUI {
		CControlUI* pControl;
	} TCFMoveUI;

	// Listener interface
	class INotifyUI {
	public:
		virtual void Notify (TNotifyUI& msg) = 0;
	};

	// MessageFilter interface
	class IMessageFilterUI {
	public:
		virtual LRESULT MessageHandler (UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
	};

	class ITranslateAccelerator {
	public:
		virtual LRESULT TranslateAccelerator (MSG *pMsg) = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//
	typedef CControlUI* (*LPCREATECONTROL)(string_view_t pstrType);

	class UILIB_API CPaintManagerUI: public CIDropTarget {
	public:
		CPaintManagerUI ();
		virtual ~CPaintManagerUI ();

	public:
		void Init (HWND hWnd, string_view_t pstrName = _T (""));
		bool IsUpdateNeeded () const;
		void NeedUpdate ();
		void Invalidate ();
		void Invalidate (RECT& rcItem);

		string_view_t GetName () const;
		HDC GetPaintDC () const;
		HWND GetPaintWindow () const;
		HWND GetTooltipWindow () const;
		int GetHoverTime () const;
		void SetHoverTime (int iTime);

		POINT GetMousePos () const;
		SIZE GetClientSize () const;
		SIZE GetInitSize ();
		void SetInitSize (int cx, int cy);
		RECT& GetSizeBox ();
		void SetSizeBox (RECT& rcSizeBox);
		RECT& GetCaptionRect ();
		void SetCaptionRect (RECT& rcCaption);
		SIZE GetRoundCorner () const;
		void SetRoundCorner (int cx, int cy);
		SIZE GetMinInfo () const;
		void SetMinInfo (int cx, int cy);
		SIZE GetMaxInfo () const;
		void SetMaxInfo (int cx, int cy);
		bool IsShowUpdateRect () const;
		void SetShowUpdateRect (bool show);
		bool IsNoActivate ();
		void SetNoActivate (bool bNoActivate);

		BYTE GetOpacity () const;
		void SetOpacity (BYTE nOpacity);

		bool IsLayered ();
		void SetLayered (bool bLayered);
		RECT& GetLayeredInset ();
		void SetLayeredInset (RECT& rcLayeredInset);
		BYTE GetLayeredOpacity ();
		void SetLayeredOpacity (BYTE nOpacity);
		string_view_t GetLayeredImage ();
		void SetLayeredImage (string_view_t pstrImage);

		CShadowUI* GetShadow ();

		void SetUseGdiplusText (bool bUse);
		bool IsUseGdiplusText () const;
		void SetGdiplusTextRenderingHint (int trh);
		int GetGdiplusTextRenderingHint () const;

		static HINSTANCE GetInstance ();
		static CDuiString GetInstancePath ();
		static CDuiString GetCurrentPath ();
		static HINSTANCE GetResourceDll ();
		static const string_view_t GetResourcePath ();
		static const string_view_t GetResourceZip ();
		static const string_view_t GetResourceZipPwd ();
		static bool IsCachedResourceZip ();
		static HANDLE GetResourceZipHandle ();
		static void SetInstance (HINSTANCE hInst);
		static void SetCurrentPath (string_view_t pStrPath);
		static void SetResourceDll (HINSTANCE hInst);
		static void SetResourcePath (string_view_t pStrPath);
		static void SetResourceZip (LPVOID pVoid, unsigned int len, string_view_t password = _T (""));
		static void SetResourceZip (string_view_t pstrZip, bool bCachedResourceZip = false, string_view_t password = _T (""));
		static void SetResourceType (int nType);
		static int GetResourceType ();
		static bool GetHSL (short* H, short* S, short* L);
		static void SetHSL (bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
		static void ReloadSkin ();
		static CPaintManagerUI* GetPaintManager (string_view_t pstrName);
		static CStdPtrArray* GetPaintManagers ();
		static bool LoadPlugin (string_view_t pstrModuleName);
		static CStdPtrArray* GetPlugins ();

		bool IsForceUseSharedRes () const;
		void SetForceUseSharedRes (bool bForce);

		void DeletePtr (void* ptr);

		DWORD GetDefaultDisabledColor () const;
		void SetDefaultDisabledColor (DWORD dwColor, bool bShared = false);
		DWORD GetDefaultFontColor () const;
		void SetDefaultFontColor (DWORD dwColor, bool bShared = false);
		DWORD GetDefaultLinkFontColor () const;
		void SetDefaultLinkFontColor (DWORD dwColor, bool bShared = false);
		DWORD GetDefaultLinkHoverFontColor () const;
		void SetDefaultLinkHoverFontColor (DWORD dwColor, bool bShared = false);
		DWORD GetDefaultSelectedBkColor () const;
		void SetDefaultSelectedBkColor (DWORD dwColor, bool bShared = false);
		TFontInfo* GetDefaultFontInfo ();
		void SetDefaultFont (string_view_t pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
		DWORD GetCustomFontCount (bool bShared = false) const;
		void AddFontArray (string_view_t pstrPath);
		HFONT AddFont (int id, string_view_t pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
		HFONT GetFont (int id);
		HFONT GetFont (string_view_t pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
		int GetFontIndex (HFONT hFont, bool bShared = false);
		int GetFontIndex (string_view_t pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
		void RemoveFont (HFONT hFont, bool bShared = false);
		void RemoveFont (int id, bool bShared = false);
		void RemoveAllFonts (bool bShared = false);
		TFontInfo* GetFontInfo (int id);
		TFontInfo* GetFontInfo (HFONT hFont);

		const TImageInfo* GetImage (string_view_t bitmap);
		const TImageInfo* GetImageEx (string_view_t bitmap, string_view_t type = _T (""), DWORD mask = 0, bool bUseHSL = false, HINSTANCE instance = NULL);
		const TImageInfo* AddImage (string_view_t bitmap, string_view_t type = _T (""), DWORD mask = 0, bool bUseHSL = false, bool bShared = false, HINSTANCE instance = NULL);
		const TImageInfo* AddImage (string_view_t bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared = false);
		const TImageInfo* AddImage (string_view_t bitmap, HBITMAP *phBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared = false);
		void RemoveImage (string_view_t bitmap, bool bShared = false);
		void RemoveAllImages (bool bShared = false);
		static void ReloadSharedImages ();
		void ReloadImages ();

		const TDrawInfo* GetDrawInfo (string_view_t pStrImage, string_view_t pStrModify);
		void RemoveDrawInfo (string_view_t pStrImage, string_view_t pStrModify);
		void RemoveAllDrawInfos ();

		void AddDefaultAttributeList (string_view_t pStrControlName, string_view_t pStrControlAttrList, bool bShared = false);
		string_view_t GetDefaultAttributeList (string_view_t pStrControlName) const;
		bool RemoveDefaultAttributeList (string_view_t pStrControlName, bool bShared = false);
		void RemoveAllDefaultAttributeList (bool bShared = false);

		void AddWindowCustomAttribute (string_view_t pstrName, string_view_t pstrAttr);
		string_view_t GetWindowCustomAttribute (string_view_t pstrName) const;
		bool RemoveWindowCustomAttribute (string_view_t pstrName);
		void RemoveAllWindowCustomAttribute ();

		// 样式管理
		void AddStyle (string_view_t pName, string_view_t pStyle, bool bShared = false);
		string_view_t GetStyle (string_view_t pName) const;
		BOOL RemoveStyle (string_view_t pName, bool bShared = false);
		const CStdStringPtrMap& GetStyles (bool bShared = false) const;
		void RemoveAllStyle (bool bShared = false);

		const TImageInfo* GetImageString (string_view_t pStrImage, string_view_t pStrModify = _T (""));

		// 初始化拖拽
		bool InitDragDrop ();
		virtual bool OnDrop (FORMATETC* pFmtEtc, STGMEDIUM& medium, DWORD *pdwEffect);

		bool AttachDialog (CControlUI* pControl);
		bool InitControls (CControlUI* pControl, CControlUI* pParent = nullptr);
		void ReapObjects (CControlUI* pControl);

		bool AddOptionGroup (string_view_t pStrGroupName, CControlUI* pControl);
		CStdPtrArray* GetOptionGroup (string_view_t pStrGroupName);
		void RemoveOptionGroup (string_view_t pStrGroupName, CControlUI* pControl);
		void RemoveAllOptionGroups ();

		CControlUI* GetFocus () const;
		void SetFocus (CControlUI* pControl);
		void SetFocusNeeded (CControlUI* pControl);

		bool SetNextTabControl (bool bForward = true);

		bool SetTimer (CControlUI* pControl, UINT nTimerID, UINT uElapse);
		bool KillTimer (CControlUI* pControl, UINT nTimerID);
		void KillTimer (CControlUI* pControl);
		void RemoveAllTimers ();

		void SetCapture ();
		void ReleaseCapture ();
		bool IsCaptured ();

		bool IsPainting ();
		void SetPainting (bool bIsPainting);

		bool AddNotifier (INotifyUI* pControl);
		bool RemoveNotifier (INotifyUI* pControl);
		void SendNotify (TNotifyUI& Msg, bool bAsync = false);
		void SendNotify (CControlUI* pControl, string_view_t pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

		bool AddPreMessageFilter (IMessageFilterUI* pFilter);
		bool RemovePreMessageFilter (IMessageFilterUI* pFilter);

		bool AddMessageFilter (IMessageFilterUI* pFilter);
		bool RemoveMessageFilter (IMessageFilterUI* pFilter);

		int GetPostPaintCount () const;
		bool IsPostPaint (CControlUI* pControl);
		bool AddPostPaint (CControlUI* pControl);
		bool RemovePostPaint (CControlUI* pControl);
		bool SetPostPaintIndex (CControlUI* pControl, int iIndex);

		int GetNativeWindowCount () const;
		RECT GetNativeWindowRect (HWND hChildWnd);
		bool AddNativeWindow (CControlUI* pControl, HWND hChildWnd);
		bool RemoveNativeWindow (HWND hChildWnd);

		void AddDelayedCleanup (CControlUI* pControl);
		void AddMouseLeaveNeeded (CControlUI* pControl);
		bool RemoveMouseLeaveNeeded (CControlUI* pControl);

		bool AddTranslateAccelerator (ITranslateAccelerator *pTranslateAccelerator);
		bool RemoveTranslateAccelerator (ITranslateAccelerator *pTranslateAccelerator);
		bool TranslateAccelerator (LPMSG pMsg);

		CControlUI* GetRoot () const;
		CControlUI* FindControl (POINT pt) const;
		CControlUI* FindControl (string_view_t pstrName) const;
		CControlUI* FindSubControlByPoint (CControlUI* pParent, POINT pt) const;
		CControlUI* FindSubControlByName (CControlUI* pParent, string_view_t pstrName) const;
		CControlUI* FindSubControlByClass (CControlUI* pParent, string_view_t pstrClass, int iIndex = 0);
		CStdPtrArray* FindSubControlsByClass (CControlUI* pParent, string_view_t pstrClass);

		static void MessageLoop ();
		static bool TranslateMessage (const LPMSG pMsg);
		static void Term ();

		CDPI* GetDPIObj ();
		void ResetDPIAssets ();
		void RebuildFont (TFontInfo* pFontInfo);
		void SetDPI (int iDPI);
		static void SetAllDPI (int iDPI);

		bool MessageHandler (UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		bool PreMessageHandler (UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		void UsedVirtualWnd (bool bUsed);

	private:
		CStdPtrArray* GetFoundControls ();
		static CControlUI* CALLBACK __FindControlFromNameHash (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromCount (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromPoint (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromTab (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromShortcut (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromName (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromClass (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlsFromClass (CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlsFromUpdate (CControlUI* pThis, LPVOID pData);

		static void AdjustSharedImagesHSL ();
		void AdjustImagesHSL ();
		void PostAsyncNotify ();

	private:
		CDuiString m_sName;
		HWND m_hWndPaint;	//所附加的窗体的句柄
		HDC m_hDcPaint;
		HDC m_hDcOffscreen;
		HDC m_hDcBackground;
		HBITMAP m_hbmpOffscreen;
		BYTE* m_pOffscreenBits;
		HBITMAP m_hbmpBackground;
		COLORREF* m_pBackgroundBits;

		// 提示信息
		HWND m_hwndTooltip;
		TOOLINFO m_ToolTip;
		int m_iHoverTime;
		bool m_bNoActivate;
		bool m_bShowUpdateRect;

		//
		CControlUI* m_pRoot;
		CControlUI* m_pFocus;
		CControlUI* m_pEventHover;
		CControlUI* m_pEventClick;
		CControlUI* m_pEventKey;
		CControlUI* m_pLastToolTip;
		//
		POINT m_ptLastMousePos = { 0 };
		SIZE m_szMinWindow = { 0 };
		SIZE m_szMaxWindow = { 0 };
		SIZE m_szInitWindowSize = { 0 };
		RECT m_rcSizeBox = { 0 };
		SIZE m_szRoundCorner = { 0 };
		RECT m_rcCaption = { 0 };
		UINT m_uTimerID;
		bool m_bFirstLayout;
		bool m_bUpdateNeeded;
		bool m_bFocusNeeded;
		bool m_bOffscreenPaint;

		BYTE m_nOpacity;
		bool m_bLayered;
		RECT m_rcLayeredInset = { 0 };
		bool m_bLayeredChanged;
		RECT m_rcLayeredUpdate = { 0 };
		TDrawInfo m_diLayered;

		bool m_bMouseTracking;
		bool m_bMouseCapture;
		bool m_bIsPainting;
		bool m_bUsedVirtualWnd;
		bool m_bAsyncNotifyPosted;

		//
		CStdPtrArray m_aNotifiers;
		CStdPtrArray m_aTimers;
		CStdPtrArray m_aTranslateAccelerator;
		CStdPtrArray m_aPreMessageFilters;
		CStdPtrArray m_aMessageFilters;
		CStdPtrArray m_aPostPaintControls;
		CStdPtrArray m_aNativeWindow;
		CStdPtrArray m_aNativeWindowControl;
		CStdPtrArray m_aDelayedCleanup;
		CStdPtrArray m_aAsyncNotify;
		CStdPtrArray m_aFoundControls;
		CStdPtrArray m_aFonts;
		CStdPtrArray m_aNeedMouseLeaveNeeded;
		CStdStringPtrMap m_mNameHash;
		CStdStringPtrMap m_mWindowCustomAttrHash;
		CStdStringPtrMap m_mOptionGroup;

		bool m_bForceUseSharedRes;
		TResInfo m_ResInfo;

		// 窗口阴影
		CShadowUI m_shadow;

		// DPI管理器
		CDPI* m_pDPI;
		// 是否开启Gdiplus
		bool m_bUseGdiplusText;
		int m_trh;
		ULONG_PTR m_gdiplusToken;
		Gdiplus::GdiplusStartupInput *m_pGdiplusStartupInput;

		// 拖拽
		bool m_bDragMode;
		HBITMAP m_hDragBitmap;

		//
		static HINSTANCE m_hInstance;
		static HINSTANCE m_hResourceInstance;
		static CDuiString m_pStrResourcePath;
		static CDuiString m_pStrResourceZip;
		static CDuiString m_pStrResourceZipPwd;
		static HANDLE m_hResourceZip;
		static bool m_bCachedResourceZip;
		static int m_nResType;
		static TResInfo m_SharedResInfo;
		static bool m_bUseHSL;
		static short m_H;
		static short m_S;
		static short m_L;
		static CStdPtrArray m_aPreMessages;
		static CStdPtrArray m_aPlugins;
	};

} // namespace DuiLib

#endif // __UIMANAGER_H__
