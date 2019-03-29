#ifndef __BIND_BASE_H__
#define __BIND_BASE_H__

#pragma once



namespace DuiLib {
	class BindCtrlBase {
		//friend class WindowImplBase;
	public:
		BindCtrlBase (faw::String ctrl_name): m_ctrl_name (ctrl_name) { /*s_bind_ctrls[m_ctrl_name.data ()] = this;*/ }
		virtual ~BindCtrlBase () {}

	protected:
		virtual faw::string_view_t GetClassType () const = 0;
		virtual void binded () {}
		CControlUI *m_ctrl = nullptr;
		faw::String m_ctrl_name;

		//static CPaintManagerUI *s_pm;
	private:
		//static std::map<faw::String, BindCtrlBase*> s_bind_ctrls;
		//static void init_binding (CPaintManagerUI *pm);
	};



	//template <typename T>
	//class BindVarBase: public BindCtrlBase {
	//public:
	//	BindVarBase (faw::string_view_t ctrl_name): BindCtrlBase (ctrl_name) {}
	//	virtual ~BindVarBase () = default;
	//	faw::string_view_t GetVarType () const;

	//	// 访问函数（未完成）
	//	//void operator= (T &o);
	//	//T &operator() ();

	//protected:
	//	void binded () override;
	//};
}

#endif //__BIND_BASE_H__
