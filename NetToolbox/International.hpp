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
			{ _T ("main.xml"),			{ _T ("main_cn.xml") } },
			{ _T ("cur_findcolor.xml"),	{ _T ("cur_findcolor_cn.xml") } },
			{ _T ("scr2gif.xml"),		{ _T ("scr2gif_cn.xml") } },
			{ _T ("menu_localnet.xml"),	{ _T ("menu_localnet_cn.xml") } }
		};
		return _src;
	}
};

#endif //__INTERNATIONAL_HPP__
