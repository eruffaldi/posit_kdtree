#include "kdtree_interface_impl.hpp"

 kdd_factory_t native_factories[] = {
	// native
	{"float", FACTORY(float) } ,
	{"double", FACTORY(double) } ,
	{"float_hamming", FACTORYH(float) } ,
	{""}

};

