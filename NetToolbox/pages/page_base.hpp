#ifndef __PAGE_BASE_HPP__
#define __PAGE_BASE_HPP__

#include <Windows.h>



class NetToolboxWnd;

class page_base {
public:
	page_base (NetToolboxWnd *parent): m_parent (parent) {}
	virtual ~page_base () = default;

	virtual void ui_update_data () {};
	virtual bool OnClick (TNotifyUI &msg) { return false; }
	virtual bool OnHeaderClick (TNotifyUI &msg) { return false; }
	virtual bool OnSelectChanged (TNotifyUI &msg) { return false; }
	virtual bool OnTextChanged (TNotifyUI &msg) { return false; }
	virtual bool OnItemSelect (TNotifyUI &msg) { return false; }
	virtual bool OnTimer (TNotifyUI &msg) { return false; }
	virtual bool is_accept_drag () { return false; }
	virtual bool OnDropFiles (LPCTSTR path) { return false; }
	virtual bool OnLButtonDown (POINT &pt) { return false; }
	virtual bool OnLButtonUp (POINT &pt) { return false; }
	virtual bool OnRButtonDown (POINT &pt) { return false; }
	virtual bool OnRButtonUp (POINT &pt) { return false; }
	virtual bool OnMouseMove (POINT &pt) { return false; }
	virtual bool OnMenuClick (MenuCmd *mc) { return false; }
protected:
	NetToolboxWnd *m_parent = nullptr;
};

#endif //__PAGE_BASE_HPP__
