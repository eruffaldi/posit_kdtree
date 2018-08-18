#include "kdtree_interface_impl.hpp"
#include "softfloat.hpp"

 kdd_factory_t softfloat_factories[] = {
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
	{"soft16", FACTORY(softfloat16) },
	0
};


//kdtree_any_list qsoftfloat(factories,sizeof(factories)/sizeof(factories[0]));