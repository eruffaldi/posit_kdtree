#include "kdtree_interface_impl.hpp"
#include "half.hpp"

 kdd_factory_t half_factories[] = {
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
	{"half", FACTORY(half_float::half) },
	{""}
};


//kdtree_any_list qsoftfloat(factories,sizeof(factories)/sizeof(factories[0]));