#ifndef __PAGE_SYS_INFO_HPP__
#define __PAGE_SYS_INFO_HPP__

#include <thread>

#include "page_base.hpp"
#include "../tools/tool_SysInfo.hpp"
#include "../tools/tool_WMI.hpp"



class page_SysInfo: public page_base {
public:
	page_SysInfo (NetToolboxWnd *parent): page_base (parent) {
		m_sysinfo_trademark->SetText (faw::Encoding::utf16_to_T (tool_SysInfo::get_trademark_name ()));
		m_sysinfo_version->SetText (tool_SysInfo::get_system_info ());
		auto[cpu1, cpu2] = tool_SysInfo::get_cpu_info ();
		m_sysinfo_cpu1->SetText (faw::Encoding::utf16_to_T (cpu1));
		m_sysinfo_cpu2->SetText (faw::Encoding::utf16_to_T (cpu2));
		m_sysinfo_mem->SetText (tool_SysInfo::get_memory_info ());
		//
		std::thread ([this] () {
			m_sysinfo_bios_id->SetText (faw::Encoding::utf16_to_T (tool_WMI::get_bios_id ()));
			m_sysinfo_cpu_id->SetText (faw::Encoding::utf16_to_T (tool_WMI::get_cpu_id ()));
			m_sysinfo_disk_id->SetText (faw::Encoding::utf16_to_T (tool_WMI::get_disk_id ()));
			m_sysinfo_mainboard_id->SetText (faw::Encoding::utf16_to_T (tool_WMI::get_mainboard_id ()));
			m_sysinfo_netcard_mac->SetText (faw::Encoding::utf16_to_T (tool_WMI::get_netcard_mac ()));
			m_sysinfo_netcard_id->SetText (faw::Encoding::utf16_to_T (tool_WMI::get_netcard_id ()));
		}).detach ();
	}
	virtual ~page_SysInfo () = default;

	void ui_update_data () override {
		m_sysinfo_mem->SetText (tool_SysInfo::get_memory_info ());
	}

protected:
	BindEditUI				m_sysinfo_trademark { _T ("sysinfo_trademark") };
	BindEditUI				m_sysinfo_version { _T ("sysinfo_version") };
	BindEditUI				m_sysinfo_cpu1 { _T ("sysinfo_cpu1") };
	BindEditUI				m_sysinfo_cpu2 { _T ("sysinfo_cpu2") };
	BindEditUI				m_sysinfo_mem { _T ("sysinfo_mem") };
	//
	BindEditUI				m_sysinfo_bios_id { _T ("sysinfo_bios_id") };
	BindEditUI				m_sysinfo_cpu_id { _T ("sysinfo_cpu_id") };
	BindEditUI				m_sysinfo_disk_id { _T ("sysinfo_disk_id") };
	BindEditUI				m_sysinfo_mainboard_id { _T ("sysinfo_mainboard_id") };
	BindEditUI				m_sysinfo_netcard_mac { _T ("sysinfo_netcard_mac") };
	BindEditUI				m_sysinfo_netcard_id { _T ("sysinfo_netcard_id") };
};

#endif //__PAGE_SYS_INFO_HPP__
