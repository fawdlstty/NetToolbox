#ifndef __UIIPADDRESS_H__
#define __UIIPADDRESS_H__

#pragma once

//给该控件添加一个属性dtstyle

namespace DuiLib {
	class CIPAddressWnd;

	/// 时间选择控件
	class UILIB_API CIPAddressUI: public CLabelUI {
		DECLARE_DUICONTROL (CIPAddressUI)

		friend class CIPAddressWnd;
	public:
		CIPAddressUI ();
		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);

		DWORD GetIP ();
		void SetIP (DWORD dwIP);

		void SetReadOnly (bool bReadOnly);
		bool IsReadOnly () const;

		void UpdateText ();

		void DoEvent (TEventUI& event);

		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

	protected:
		DWORD			m_dwIP;
		bool			m_bReadOnly		= false;
		int				m_nIPUpdateFlag;

		CIPAddressWnd	*m_pWindow		= nullptr;
	};
}
#endif // __UIIPADDRESS_H__