#include "kdtree_interface_impl.hpp"
#include "softposit_cpp.h"


/*
Requires
	explicit operator double() const { return toDouble(); }

	explicit operator float() const { return toDouble(); }
*/

kdd_factory_t softposit_factories[] = {
	// tabulated
	{"softposit8_0", FACTORY(posit8) } ,
	{"softposit16_1", FACTORY(posit16) } ,
	{"softposit32_2", FACTORY(posit32) } ,
	{"" }
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
};
