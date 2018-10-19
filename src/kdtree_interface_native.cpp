#include "kdtree_interface_impl.hpp"

 kdd_factory_t native_factories[] = {
	// native
	{"float32.native", FACTORY(float) } ,
	{"float64.native", FACTORY(double) } ,
	//{"float_hamming", FACTORYH(float) } ,
	{""}

};

