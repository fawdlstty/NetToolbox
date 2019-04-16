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

		};
	}
};

#endif //__INTERNATIONAL_HPP__
