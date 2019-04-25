#ifndef __PAGE_ENCDEC_HPP__
#define __PAGE_ENCDEC_HPP__


#include "page_base.hpp"



class page_EncDec: public page_base {
public:
	page_EncDec (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_EncDec () = default;

	bool OnClick (TNotifyUI& msg) override {
		 faw::String name = msg.pSender->GetName ();
		if (name == _T ("encdec_enc")) {
			faw::String _src = m_encdec_data->GetText (), t_data;
			std::string data_gb18030 = _src.stra ();
			std::string data_utf8 = faw::Encoding::T_to_utf8 (_src.str_view ());
			t_data = faw::Encoding::gb18030_to_T (faw::Encoding::percent_str_encode (data_utf8));
			m_encdec_percent->SetText (t_data.str_view ());
			t_data = faw::Encoding::gb18030_to_T (faw::Encoding::escape_x_str_encode (data_utf8));
			m_encdec_escape_x->SetText (t_data.str_view ());
			t_data = faw::Encoding::gb18030_to_T (faw::Encoding::escape_u_str_encode (data_gb18030));
			m_encdec_escape_u->SetText (t_data.str_view ());
			t_data = faw::Encoding::gb18030_to_T (faw::Encoding::base64_encode (data_utf8));
			m_encdec_base64->SetText (t_data.str_view ());
			return true;
		} else if (name == _T ("encdec_dec")) {
			std::string data = faw::Encoding::T_to_gb18030 (m_encdec_data->GetText ().str_view ());
			faw::String t_data;
			if (faw::Encoding::is_percent_str (data)) {
				t_data = faw::Encoding::utf8_to_T (faw::Encoding::percent_str_decode (data));
			} else {
				t_data = _T ("(Decoding failure)");
			}
			m_encdec_percent->SetText (t_data.str_view ());
			if (faw::Encoding::is_escape_x_str (data)) {
				t_data = faw::Encoding::utf8_to_T (faw::Encoding::escape_x_str_decode (data));
			} else {
				t_data = _T ("(Decoding failure)");
			}
			m_encdec_escape_x->SetText (t_data.str_view ());
			if (faw::Encoding::is_escape_u_str (data)) {
				t_data = faw::Encoding::gb18030_to_T (faw::Encoding::escape_u_str_decode (data));
			} else {
				t_data = _T ("(Decoding failure)");
			}
			m_encdec_escape_u->SetText (t_data.str_view ());
			if (faw::Encoding::is_base64_str (data)) {
				t_data = faw::Encoding::gb18030_to_T (faw::Encoding::base64_decode (data));
			} else {
				t_data = _T ("(Decoding failure)");
			}
			m_encdec_base64->SetText (t_data.str_view ());
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
