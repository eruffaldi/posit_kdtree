#include "kdtree_interface_impl.hpp"
#include "float16native32.hpp"

 kdd_factory_t native_factories[] = {
	// float16 built using native implementation
	{"float16.native32", FACTORY(float16n32) } ,
	{"float32.native", FACTORY(float) } ,
	{"float64.native", FACTORY(double) } ,
	//{"float_hamming", FACTORYH(float) } ,
	{""}

};

