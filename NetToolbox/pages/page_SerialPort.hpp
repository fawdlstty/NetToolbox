#ifndef __PAGE_SERIAL_PORT_HPP__
#define __PAGE_SERIAL_PORT_HPP__

#include <vector>
#include <string>
#include <thread>
#include <tuple>
#include <memory>
#include <functional>
#include <exception>

#include "page_base.hpp"
#include "../tools/tool_String.hpp"
#include "../tools/tool_SerialPort.hpp"



class page_SerialPort: public page_base {
	enum SerialDataType {
		SerialDataTypeInfo = DT_CENTER,
		SerialDataTypeSend = DT_RIGHT,
		SerialDataTypeRecv = DT_LEFT
	};

public:
	page_SerialPort (NetToolboxWnd *parent): page_base (parent) {
		m_serial.set_on_event (std::bind (&page_SerialPort::on_receive, this, std::placeholders::_1), std::bind (&page_SerialPort::on_close, this));
		//
		static std::vector<LPCTSTR> vboudrate { _T ("110"), _T ("300"), _T ("600"), _T ("1200"), _T ("2400"), _T ("4800"), _T ("9600"), _T ("14400"), _T ("19200"), _T ("38400"), _T ("56000"), _T ("57600"), _T ("115200"), _T ("128000"), _T ("256000") };
		for (auto bd : vboudrate)
			m_serial_boudrate->Add (new CListLabelElementUI (bd));
		m_serial_boudrate->SetText (_T ("9600"));
		m_serial_boudrate->SetDropBoxSize ({ 0, (LONG) vboudrate.size () * 20 + 2 });
		//
		static std::vector<LPCTSTR> vbytesize { _T ("1"), _T ("2"), _T ("3"), _T ("4"), _T ("5"), _T ("6"), _T ("7"), _T ("8") };
		for (auto bs : vbytesize)
			m_serial_bytesize->Add (new CListLabelElementUI (bs));
		m_serial_bytesize->SetText (_T ("8"));
		m_serial_bytesize->SetDropBoxSize ({ 0, (LONG) vbytesize.size () * 20 + 2 });
		//
		static std::vector<LPCTSTR> vparity { _T ("none"), _T ("odd"), _T ("even"), _T ("mark"), _T ("space") };
		for (auto p : vparity)
			m_serial_parity->Add (new CListLabelElementUI (p));
		m_serial_parity->SetText (_T ("none"));
		m_serial_parity->SetDropBoxSize ({ 0, (LONG) vparity.size () * 20 + 2 });
		//
		static std::vector<LPCTSTR> vstopbits { _T ("1"), _T ("1.5"), _T ("2") };
		for (auto sb : vstopbits)
			m_serial_stopbits->Add (new CListLabelElementUI (sb));
		m_serial_stopbits->SetText (_T ("1"));
		m_serial_stopbits->SetDropBoxSize ({ 0, (LONG) vstopbits.size () * 20 + 2 });
	}
	virtual ~page_SerialPort () = default;

	void ui_update_data () override {
		static auto _is_equal = [] (std::vector<std::string>& v1, std::vector<std::string>& v2) {
			if (v1.size () != v2.size ())
				return false;
			for (size_t i = 0; i < v1.size (); ++i) {
				if (v1[i] != v2[i])
					return false;
			}
			return true;
		};
		static auto _index_arr = [] (std::vector<std::string>& v, std::string str) {
			for (size_t i = 0; i < v.size (); ++i) {
				if (str == _serial_name (v[i]))
					return i;
			}
			return std::string::npos;
		};

		static std::vector<std::string> s_vSp;
		auto _vSp = tool_SerialPort::get_list ();
		if (_is_equal (s_vSp, _vSp))
			return;
		if (m_serial.is_open () && !_index_arr (_vSp, m_serial.get_name ())) {
			m_serial.close ();
		}
		s_vSp = _vSp;
		m_serial_name->RemoveAll ();
		for (auto name : s_vSp) {
			m_serial_name->Add (new CListLabelElementUI (faw::Encoding::gb18030_to_T (name).c_str (), 20));
		}
		if (s_vSp.size () > 0)
			m_serial_name->SelectItem (0);
		m_serial_name->SetDropBoxSize ({ 0, (LONG) s_vSp.size () * 20 + 2 });
	}

	bool on_send () {
		if (m_tmp_port_name == "")
			return false;
		 faw::String data = m_serial_senddata->GetText ();
		std::string _data = data.stra ();
		if (!append_data (SerialDataTypeSend, _data, !m_serial_hex->IsSelected ()))
			return false;
		if (m_serial_newline->IsSelected ())
			_data += "\r\n";
		m_serial.write (_data);
		return true;
	}

	void on_receive (std::string data) {
		append_data (SerialDataTypeRecv, data);
	}

	void on_close () {
		enable_combos (true);
		append_data (SerialDataTypeInfo, tool_StringA::format ("串口已主动断开：<c #7f0000>%s</c>", m_tmp_port_name.c_str ()));
		m_tmp_port_name = "";
	}

	bool OnClick (TNotifyUI& msg) override {
		try {
			 faw::String name = msg.pSender->GetName ();
			if (name == _T ("serial_btnopen")) {
				if (!m_serial.is_open () && m_tmp_port_name == "") {
					m_tmp_port_name = m_serial_name->GetText ().stra ();
					std::string _parity = m_serial_parity->GetText ().stra ();
					std::string _stopbits = m_serial_stopbits->GetText ().stra ();
					if (m_tmp_port_name == "") {
						m_parent->invoke ([this] () -> LRESULT {
							m_parent->show_status (NetToolboxWnd::StatusIcon::Error, _T ("打开串口失败：未选择可用的串口"));
							append_data (SerialDataTypeInfo, "打开串口失败：未选择可用的串口");
							return 0;
						});
						return true;
					}
					uint32_t _baud_rate = _ttoi (m_serial_boudrate->GetText ().c_str ());
					uint8_t _byte_size = _ttoi (m_serial_bytesize->GetText ().c_str ());
					m_serial.open (_serial_name (m_tmp_port_name), _baud_rate, _byte_size, _parity, _stopbits);
					if (m_serial.is_open ()) {
						enable_combos (false);
						append_data (SerialDataTypeInfo, tool_StringA::format ("已打开串口：<c #00007f>%s</c>", m_tmp_port_name.c_str ()));
					}
				}
				return true;
			} else if (name == _T ("serial_btnclose")) {
				if (m_serial.is_open ()) {
					m_serial.close ();
					enable_combos (true);
					append_data (SerialDataTypeInfo, tool_StringA::format ("已关闭串口：<c #7f0000>%s</c>", m_tmp_port_name.c_str ()));
					m_tmp_port_name = "";
				}
				return true;
			} else if (name == _T ("serial_btnsend")) {
				if (on_send () && m_serial_repeat->IsSelected () && !m_repeat_timer) {
					UINT nElapse = _ttoi (m_serial_repeat_mill->GetText ().c_str ());
					m_serial_repeat->SetTimer ((UINT) *m_serial_repeat, (nElapse > 0 ? nElapse : 1));
					m_repeat_timer = true;
				}
				return true;
			} else if (name == _T ("serial_newline")) {
				return true;
			}
			return false;
		} catch (std::exception &e) {
			m_parent->show_status (NetToolboxWnd::StatusIcon::Error, faw::Encoding::gb18030_to_T (e.what ()).c_str ());
			return true;
		}
	}

	bool OnSelectChanged (TNotifyUI& msg) override {
		 faw::String name = msg.pSender->GetName ();
		if (name == _T ("serial_hex")) {
			bool is_source = !m_serial_hex->IsSelected ();
			for (size_t i = 0; i < m_data.size (); ++i) {
				auto[type, source, hex] = m_data[i];
				if (type != SerialDataTypeInfo) {
					BindTextUI ctrl { faw::String::format (_T ("serial_cnt_%d"), i) };
					ctrl->SetText ((is_source ? source : hex).c_str ());
				}
			}
			return true;
		} else if (name == _T ("serial_repeat")) {
			bool is_repeat = m_serial_repeat->IsSelected ();
			if (!m_serial_repeat->IsSelected () && m_repeat_timer) {
				m_serial_repeat->KillTimer ((UINT) *m_serial_repeat);
				m_repeat_timer = false;
			}
			return true;
		}
		return false;
	}

	bool OnTimer (TNotifyUI& msg) override {
		 faw::String name = msg.pSender->GetName ();
		if (name == _T ("serial_repeat")) {
			on_send ();
			return true;
		}
		return false;
	}

protected:
	bool append_data (SerialDataType type, std::string data, bool is_source = true) {
		std::string hex_data;
		static auto _get_hex = [] (unsigned char c) -> char { c &= 0xf; if (c < 10) return c+'0'; if (c < 16) return c-10+'a'; return '?'; };
		static auto _is_hex = [] (char c) -> bool { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); };
		static auto _from_hex = [] (char c) -> unsigned char { if (c >= '0' && c <= '9') return c-'0'; if (c >= 'A' && c <= 'F') return c+10-'A'; if (c >= 'a' && c <= 'f') return c+10-'a'; return '\0'; };
		if (is_source) {
			for (auto c : data) {
				hex_data += _get_hex (c >> 4);
				hex_data += _get_hex (c & 0xf);
			}
		} else {
			hex_data = data;
			data.clear ();
			for (size_t i = 0; i < hex_data.size () - 1; i += 2) {
				unsigned char c1 = hex_data[i], c2 = hex_data[i];
				while ((!_is_hex (c1) || !_is_hex (c2)) && i < hex_data.size () - 1)
					++i;
				if (i == hex_data.size ()) {
					break;
				} else if (i == hex_data.size () - 1) {
					return false;
				}
				char c = (_from_hex (c1) << 4) | _from_hex (c2);
				data += c;
			}
		}
		faw::String _data = faw::Encoding::gb18030_to_T (data).c_str ();
		faw::String _hex_data = faw::Encoding::gb18030_to_T (hex_data).c_str ();
		m_data.push_back ({ type, _data, _hex_data });
		auto ctrl = new CTextUI ();
		ctrl->SetName (faw::String::format (_T ("serial_cnt_%d"), m_data.size () - 1).c_str ());
		ctrl->SetText ((m_serial_hex->IsSelected () && type != SerialDataTypeInfo ? _hex_data : _data).c_str ());
		ctrl->SetBkColor (0xFFCCCCCC);
		ctrl->SetAutoCalcWidth (true);
		ctrl->SetAutoCalcHeight (true);
		RECT rcPadding = { (type == SerialDataTypeSend ? 100 : 0), 0, (type == SerialDataTypeRecv ? 100 : 0), 10 };
		ctrl->SetPadding (rcPadding);
		ctrl->SetFloatAlign (type);
		if (type != SerialDataTypeInfo) {
			ctrl->SetTextColor (0xFF333333);
			ctrl->SetTextPadding ({ 10, 10, 10, 10 });
			ctrl->SetBorderRound ({ 20, 20 });
		} else {
			ctrl->SetTextColor (0xFF777777);
			ctrl->SetTextPadding ({ 5, 5, 5, 5 });
			ctrl->SetBorderRound ({ 10, 10 });
			ctrl->SetShowHtml (true);
		}
		SIZE range = m_serial_cnt->GetScrollRange ();
		SIZE pos = m_serial_cnt->GetScrollPos ();
		m_serial_cnt->Add (ctrl);
		if (range.cy < pos.cy + 10) {
			std::thread ([this] () {
				::Sleep (10);
				m_parent->invoke ([this] () -> LRESULT {
					m_serial_cnt->SetScrollPos (m_serial_cnt->GetScrollRange ());
					return 0;
				});
			}).detach ();
		}
		return true;
	}

	void enable_combos (bool enable) {
		m_serial_name->SetEnabled (enable);
		m_serial_boudrate->SetEnabled (enable);
		m_serial_bytesize->SetEnabled (enable);
		m_serial_parity->SetEnabled (enable);
		m_serial_stopbits->SetEnabled (enable);
	}

	static std::string _serial_name (std::string s) {
		size_t p1 = s.find ('(') + 1, p2 = s.find (')');
		return s.substr (p1, p2 - p1);
	}

	BindComboUI				m_serial_name { _T ("serial_name") };
	BindComboUI				m_serial_boudrate { _T ("serial_boudrate") };
	BindComboUI				m_serial_bytesize { _T ("serial_bytesize") };
	BindComboUI				m_serial_parity { _T ("serial_parity") };
	BindComboUI				m_serial_stopbits { _T ("serial_stopbits") };
	BindCheckBoxUI			m_serial_hex { _T ("serial_hex") };
	BindEditUI				m_serial_repeat_mill { _T ("serial_repeat_mill") };
	BindCheckBoxUI			m_serial_repeat { _T ("serial_repeat") };
	BindCheckBoxUI			m_serial_newline { _T ("serial_newline") };
	BindVerticalLayoutUI	m_serial_cnt { _T ("serial_cnt") };
	BindRichEditUI			m_serial_senddata { _T ("serial_senddata") };
	tool_SerialPort			m_serial;
	std::string				m_tmp_port_name			= "";
	bool					m_repeat_timer			= false;
	std::vector<std::tuple<SerialDataType, faw::String, faw::String>>	m_data;
};

#endif //__PAGE_SERIAL_PORT_HPP__
