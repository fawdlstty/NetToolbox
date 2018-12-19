#ifndef __PAGE_EXAMPLE_HPP__
#define __PAGE_EXAMPLE_HPP__

#include "page_base.hpp"



class page_Example: public page_base {
public:
	page_Example (NetToolboxWnd *parent): page_base (parent) {}
	virtual ~page_Example () = default;
};

#endif //__PAGE_EXAMPLE_HPP__
