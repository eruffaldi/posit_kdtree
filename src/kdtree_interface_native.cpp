#include "kdtree_interface_impl.hpp"

// TODO move inanothermodule 
#include "float16native32.hpp"
#include "anyfloat.hpp"


 kdd_factory_t native_factories[] = {
	// float16 built using native implementation
	{"float16.native32", FACTORY(float16n32) } ,
	{"float16.emu", FACTORY(binary16_emu) } ,
	{"float32.emu", FACTORY(binary32_emu) } ,
	{"float8.emu", FACTORY(binary8_emu) } ,
	{"float32.native", FACTORY(float) } ,
	{"float64.native", FACTORY(double) } ,
	//{"float_hamming", FACTORYH(float) } ,
	{""}

};

