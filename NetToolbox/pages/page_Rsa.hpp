#ifndef __PAGE_RSA_HPP__
#define __PAGE_RSA_HPP__

#include "page_base.hpp"
#include "../tools/tool_SysInfo.hpp"
#include "../tools/tool_Path.hpp"
#include "../tools/tool_String.hpp"
#include "../tools/tool_Rsa.hpp"
#include "../tools/tool_Encoding.hpp"



class page_Rsa: public page_base {
public:
	page_Rsa (NetToolboxWnd *parent): page_base (parent) {
		string_t path = tool_Path::get_exe_path ();
		m_rsa_evp_pubkey->SetText (tool_StringT::format (_T ("%c:\\evp_pubkey.key"), path[0]));
		m_rsa_rsa_pubkey->SetText (tool_StringT::format (_T ("%c:\\rsa_pubkey.key"), path[0]));
		m_rsa_rsa_prvkey->SetText (tool_StringT::format (_T ("%c:\\rsa_prvkey.key"), path[0]));
	}
	virtual ~page_Rsa () = default;

	bool OnClick (TNotifyUI &msg) override {
		if (msg.pSender->GetName () == _T ("rsa_generate")) {
			bool succ = tool_Rsa::generate (
				_ttoi (m_rsa_gensize->GetText ().c_str ()),
				(m_rsa_selevp_pubkey->IsSelected () ? tool_Encoding::get_gb18030 (m_rsa_evp_pubkey->GetText ()) : ""),
				(m_rsa_selrsa_pubkey->IsSelected () ? tool_Encoding::get_gb18030 (m_rsa_rsa_pubkey->GetText ()) : ""),
				tool_Encoding::get_gb18030 (m_rsa_rsa_prvkey->GetText ()));
			if (succ)
				m_parent->show_status (NetToolboxWnd::StatusIcon::Ok, _T ("RSA公私钥导出成功！"));
			else
				m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _T ("RSA公私钥导出失败！"));
			return true;
		}
		return false;
	}

	bool OnSelectChanged (TNotifyUI &msg) override {
		if (msg.pSender->GetName () == _T ("rsa_selevp_pubkey")) {
			m_rsa_evp_pubkey->SetEnabled (true);
			m_rsa_rsa_pubkey->SetEnabled (false);
			return true;
		} else if (msg.pSender->GetName () == _T ("rsa_selrsa_pubkey")) {
			m_rsa_evp_pubkey->SetEnabled (false);
			m_rsa_rsa_pubkey->SetEnabled (true);
			return true;
		}
		return false;
	}

protected:
	BindComboUI		m_rsa_gensize { _T ("rsa_gensize") };
	BindOptionUI	m_rsa_selevp_pubkey { _T ("rsa_selevp_pubkey") };
	BindEditUI		m_rsa_evp_pubkey { _T ("rsa_evp_pubkey") };
	BindOptionUI	m_rsa_selrsa_pubkey { _T ("rsa_selrsa_pubkey") };
	BindEditUI		m_rsa_rsa_pubkey { _T ("rsa_rsa_pubkey") };
	BindEditUI		m_rsa_rsa_prvkey { _T ("rsa_rsa_prvkey") };
};

#endif //__PAGE_RSA_HPP__
