#include "kdtree_interface_impl.hpp"
#include "softposit_cpp.h"


/*
Requires
	explicit operator double() const { return toDouble(); }

	explicit operator float() const { return toDouble(); }
*/

kdd_factory_t softposit_factories[] = {
	// tabulated
	{"posit8_0.soft", FACTORY(posit8) } ,
	{"posit16_0.soft", FACTORY(posit16) } ,
	{"posit32_2.soft", FACTORY(posit32) } ,
	{"" }
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
};
