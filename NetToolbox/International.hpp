#ifndef __INTERNATIONAL_HPP__
#define __INTERNATIONAL_HPP__

#include <vector>
#include <map>
#include <Windows.h>
#include <tchar.h>

#include "../DuiLib/3rd/FawLib/FawLib.hpp"

class International {
public:
	static faw::string_view_t translate (faw::string_view_t _src) {
		static LCID _lcid = ::GetSystemDefaultLCID ();
		static std::map<faw::string_view_t, std::vector<faw::string_view_t>> _datum = {
			{ _T ("main.xml"),										{ _T ("main_cn.xml") } },
			{ _T ("cur_findcolor.xml"),								{ _T ("cur_findcolor_cn.xml") } },
			{ _T ("scr2gif.xml"),									{ _T ("scr2gif_cn.xml") } },
			{ _T ("menu_localnet.xml"),								{ _T ("menu_localnet_cn.xml") } },
			{ _T ("Info"),											{ _T ("提示") } },
			{ _T ("Date"),											{ _T ("日期") } },
			{ _T ("Time"),											{ _T ("时间") } },
			{ _T ("Phone"),											{ _T ("手机") } },
			{ _T ("Money"),											{ _T ("金额") } },
			{ _T ("Domain"),										{ _T ("域名") } },
			{ _T ("E-mail"),										{ _T ("电子邮箱") } },
			{ _T ("ID No."),										{ _T ("身份证号") } },
			{ _T ("Postcode"),										{ _T ("邮编") } },
			{ _T ("Process ID"),									{ _T ("进程PID") } },
			{ _T ("Process Name"),									{ _T ("进程名称") } },
			{ _T ("GIF Record"),									{ _T ("GIF录制") } },
			{ _T ("Local Addr"),									{ _T ("本地地址") } },
			{ _T ("Local Port"),									{ _T ("本地端口") } },
			{ _T ("Remote Addr"),									{ _T ("远程地址") } },
			{ _T ("Remote Port"),									{ _T ("远程端口") } },
			{ _T ("TCP Connected"),									{ _T ("TCP已连接") } },
			{ _T ("TCP Listening"),									{ _T ("TCP正在监听") } },
			{ _T ("Unknown Error."),								{ _T ("未知错误。") } },
			{ _T ("Tracert Complete."),								{ _T ("路由跟踪完成。") } },
			{ _T ("Total %s / Available %s"),						{ _T ("总共 %s / 可用 %s") } },
			{ _T ("Chinese Character(GB18030)"),					{ _T ("汉字(GB18030)") } },
			{ _T ("Create Process Failed, %s"),						{ _T ("创建进程失败，%s") } },
			{ _T ("Process could not terminate!"),					{ _T ("进程无法结束！") } },
			{ _T ("GIF Image exported failure!"),					{ _T ("GIF图导出失败！") } },
			{ _T ("GIF Image exported successfully!"),				{ _T ("GIF图导出成功！") } },
			{ _T ("Are you sure you want to terminate the process %s (pid:%d)?"), { _T ("确定要结束进程 %s (pid:%d) 吗？") } },
			{ _T ("The following file already exists. Do you want to overwrite it?"), { _T ("以下文件已存在，是否覆盖？") } },
			{ _T ("The following file already exists. Do you want to overwrite it?\n%s"), { _T ("以下文件已存在，是否覆盖？\n%s") } },
			{ _T ("Routing trace has ended and the maximum traceable hop count has been exceeded."), { _T ("路由跟踪结束，已超过可跟踪的最大跃点数。") } },
			{ _T ("Insufficient permissions. Do you want to reopen the program with administrator permissions?"), { _T ("权限不足，是否以管理员权限重新打开程序？") } },
		};
		return _src;
	}
#ifdef _UNICODE
	static std::string translate (std::string_view _src) {
		faw::String _str = _src;
		_str = translate (_str.str_view ());
		return _str.stra ();
	}
#else
	static std::wstring_view translate (std::wstring_view _src) {
		faw::String _str = _src;
		_str = translate (_str.str_view ());
		return _str.strw ();
	}
#endif
};

#endif //__INTERNATIONAL_HPP__
