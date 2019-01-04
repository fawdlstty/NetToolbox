#ifndef __PAGE_REGEX_HPP__
#define __PAGE_REGEX_HPP__

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "page_base.hpp"
#include "../tools/tool_String.hpp"
#include "../tools/tool_Encoding.hpp"
#include "../tools/tool_Formatting.hpp"



class page_Regex: public page_base {
public:
	page_Regex (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_Regex () = default;

	bool OnClick (TNotifyUI& msg) override {
		CDuiString name = msg.pSender->GetName ();
		if (name == _T ("regex_match")) {
			std::string _statement = tool_Encoding::T_to_gb18030 (m_regex_statement->GetText ());
			tool_StringA::replace (_statement, "\r\n", "\n");
			tool_StringA::replace (_statement, "\r", "\n");
			std::string _content = tool_Encoding::T_to_gb18030 (m_regex_content->GetText ());
			tool_StringA::replace (_content, "\r\n", "\n");
			tool_StringA::replace (_content, "\r", "\n");
			auto[_err, _vresult] = tool_StringT::match_regex (_statement, _content);
			if (!_err.empty ())
				m_parent->show_status (NetToolboxWnd::StatusIcon::Warning, tool_Encoding::gb18030_to_T (_err));
			std::string _result = "";
			for (auto &_item : _vresult) {
				_result += _item;
				_result += '\n';
			}
			string_t t_result = tool_Encoding::gb18030_to_T (_result);
			m_regex_result->SetText (t_result.c_str ());
			return true;
		}
		return false;
	}

	bool OnItemSelect (TNotifyUI& msg) override {
		static std::function<string_t (string_t)> _query_regex = [] (string_t desp) {
			return tool_Encoding::gb18030_to_T (tool_Formatting::query_regex (tool_Encoding::T_to_gb18030 (desp)));
		};
		CDuiString name = msg.pSender->GetName ();
		if (name == _T ("regex_combo_net")) {
			m_regex_statement->SetText (_query_regex (m_regex_combo_net->GetText ()));
			m_regex_combo_net->SetText (_T ("网络类"));
			return true;
		} else if (name == _T ("regex_combo_format")) {
			m_regex_statement->SetText (_query_regex (m_regex_combo_format->GetText ()));
			m_regex_combo_format->SetText (_T ("格式类"));
		}
		return false;
	}

protected:
	BindComboUI				m_regex_combo_net { _T ("regex_combo_net") };
	BindComboUI				m_regex_combo_format { _T ("regex_combo_format") };
	BindRichEditUI			m_regex_statement { _T ("regex_statement") };
	BindRichEditUI			m_regex_content { _T ("regex_content") };
	BindRichEditUI			m_regex_result { _T ("regex_result") };
};

#endif //__PAGE_REGEX_HPP__
