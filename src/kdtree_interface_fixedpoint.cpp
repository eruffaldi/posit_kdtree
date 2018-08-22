#include "kdtree_interface_impl.hpp"
#include <sg14/fixed_point>


#define FIXEDFACTORY(T,frac) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<sg14::fixed_point\
<T,frac> ,nanoflann::L2_Adaptor>(); }) 


 kdd_factory_t fixedpoint_factories[] = {
	{"fixed32_16", FIXEDFACTORY(int32_t,-16) },
	{"fixed16_8", FIXEDFACTORY(int16_t,-8) },
	{""}
};


//kdtree_any_list qsoftfloat(factories,sizeof(factories)/sizeof(factories[0]));