#ifndef __PAGE_ENCDEC_HPP__
#define __PAGE_ENCDEC_HPP__

#include "../tools/tool_Encoding.hpp"
#include "../tools/tool_Base64.hpp"

#include "page_base.hpp"



class page_EncDec: public page_base {
public:
	page_EncDec (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_EncDec () = default;

	bool OnClick (TNotifyUI& msg) override {
		CDuiString name = msg.pSender->GetName ();
		if (name == _T ("encdec_enc")) {
			string_t _src = m_encdec_data->GetText (), t_data;
			std::string data_gb18030 = tool_Encoding::get_gb18030 (_src);
			std::string data_utf8 = tool_Encoding::get_utf8 (_src);
			t_data = tool_Encoding::get_T (tool_Encoding::percent_str_encode (data_utf8));
			m_encdec_percent->SetText (t_data);
			t_data = tool_Encoding::get_T (tool_Encoding::escape_x_str_encode (data_utf8));
			m_encdec_escape_x->SetText (t_data);
			t_data = tool_Encoding::get_T (tool_Encoding::escape_u_str_encode (data_gb18030));
			m_encdec_escape_u->SetText (t_data);
			t_data = tool_Encoding::get_T (tool_Base64::base64_encode ((const unsigned char*) data_utf8.c_str (), data_utf8.size ()));
			m_encdec_base64->SetText (t_data);
			return true;
		} else if (name == _T ("encdec_dec")) {
			std::string data = tool_Encoding::get_gb18030 (m_encdec_data->GetText ());
			string_t t_data;
			if (tool_Encoding::is_percent_str (data)) {
				t_data = tool_Encoding::get_T_from_utf8 (tool_Encoding::percent_str_decode (data));
			} else {
				t_data = _T ("(½âÂëÊ§°Ü)");
			}
			m_encdec_percent->SetText (t_data);
			if (tool_Encoding::is_escape_x_str (data)) {
				t_data = tool_Encoding::get_T_from_utf8 (tool_Encoding::escape_x_str_decode (data));
			} else {
				t_data = _T ("(½âÂëÊ§°Ü)");
			}
			m_encdec_escape_x->SetText (t_data);
			if (tool_Encoding::is_escape_u_str (data)) {
				t_data = tool_Encoding::get_T (tool_Encoding::escape_u_str_decode (data));
			} else {
				t_data = _T ("(½âÂëÊ§°Ü)");
			}
			m_encdec_escape_u->SetText (t_data);
			if (tool_Base64::is_base64 ((const unsigned char*) data.c_str (), data.size ())) {
				t_data = tool_Encoding::get_T (tool_Base64::base64_decode (data));
			} else {
				t_data = _T ("(½âÂëÊ§°Ü)");
			}
			m_encdec_base64->SetText (t_data);
			return true;
		}
		return false;
	}

private:
	BindRichEditUI			m_encdec_data { _T ("encdec_data") };
	BindRichEditUI			m_encdec_percent { _T ("encdec_percent") };
	BindRichEditUI			m_encdec_escape_x { _T ("encdec_escape_x") };
	BindRichEditUI			m_encdec_escape_u { _T ("encdec_escape_u") };
	BindRichEditUI			m_encdec_base64 { _T ("encdec_base64") };
	size_t					m_val_index = 0;
};

#endif //__PAGE_ENCDEC_HPP__
