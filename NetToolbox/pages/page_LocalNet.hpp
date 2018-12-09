#ifndef __PAGE_LOCAL_NET_HPP__
#define __PAGE_LOCAL_NET_HPP__

#include "page_base.hpp"
#include "../tools/tool_NetInfo.hpp"
#include "../tools/tool_Encoding.hpp"



class page_LocalNet: public page_base {
public:
	page_LocalNet (NetToolboxWnd *parent): page_base (parent) { ui_update_data (); }

	virtual ~page_LocalNet () = default;

	void ui_update_data () override {
		static size_t x = 0;
		if (x++ % 5 > 0)
			return;
		static std::vector<std::tuple<std::string, std::string, std::string, bool>> vlocal_net;
		auto _vlocal_net = tool_NetInfo::get_local_net ();
		bool bIsEqual = (vlocal_net.size () == _vlocal_net.size ());
		if (bIsEqual) {
			for (size_t i = 0; i < vlocal_net.size (); ++i) {
				if (vlocal_net[i] != _vlocal_net[i]) {
					bIsEqual = false;
					break;
				}
			}
		}
		if (!bIsEqual) {
			m_localnet_list->RemoveAll ();
			vlocal_net = _vlocal_net;
			for (size_t i = 0; i < vlocal_net.size (); ++i) {
				auto[desc, local, gateway, is_dhcp] = vlocal_net[i];
				CListContainerElementUI *item = new CListContainerElementUI ();
				item->SetFixedHeight (20);
				//
				CControlUI *ctrl = new CContainerUI ();
				ctrl->SetBkImage (_T ("file='list_ico_0.png' source='0,0,16,16' dest='0,0,16,16'"));
				ctrl->SetAttribute (_T ("padding"), _T ("7,2,7,2"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (tool_Encoding::gb18030_to_utf16 (desc).c_str ());
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("10,4,0,4"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (tool_Encoding::gb18030_to_utf16 (local).c_str ());
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (tool_Encoding::gb18030_to_utf16 (gateway).c_str ());
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				//
				ctrl = new CTextUI ();
				ctrl->SetText (is_dhcp ? _T ("已启用") : _T ("已禁用"));
				ctrl->SetAttribute (_T ("align"), _T ("center"));
				ctrl->SetAttribute (_T ("padding"), _T ("0,4,0,4"));
				item->Add (ctrl);
				m_localnet_list->Add (item);
			}
		}
	}

protected:
	BindListUI			m_localnet_list { _T ("localnet_list") };
};

#endif //__PAGE_LOCAL_NET_HPP__
