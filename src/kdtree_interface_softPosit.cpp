#include "kdtree_interface_impl.hpp"
#include "softposit_cpp.h"


/*
Requires
	explicit operator double() const { return toDouble(); }

	explicit operator float() const { return toDouble(); }
*/

kdd_factory_t cppPosit_factories[] = {
	// tabulated
	{"softposit8", FACTORY(posit8) } ,
	{"softposit16", FACTORY(posit16) } ,
	{"" }
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
};
