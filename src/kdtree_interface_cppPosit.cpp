#include "kdtree_interface_impl.hpp"
#include "posit.h"
#include "posit12.hpp"
//#include "posit16.hpp"
#include "posit10.hpp"
#include "posit8.hpp"
#include "positf.h"

#define FACTORYPOSIT(stype,bits,esbits,ftype,withnan) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<Posit<stype,bits,esbits,ftype,withnan>,nanoflann::L2_Adaptor >(); }) 
#define FACTORYPOSITH(stype,bits,esbits,ftype,withnan) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<Posit<stype,bits,esbits,ftype,withnan>,Hamming_Adaptor >(); }) 
#define FACTORYPOSITF(stype,bits,esbits,ftype,withnan) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<PositF<stype,bits,esbits,ftype,withnan>,nanoflann::L2_Adaptor >(); }) 

kdd_factory_t cppPosit_factories[] = {

	// like the soft posit
	{"posit8_0", FACTORYPOSIT(int8_t, 8, 1, uint32_t, false) },
	{"posit16_1", FACTORYPOSIT(int16_t, 16, 1, uint32_t, false) },
	{"posit32_2", FACTORYPOSIT(int32_t, 32, 2, uint64_t, false) },
	
	{"posit8_0f", FACTORYPOSITF(int8_t, 8, 1, float, false) },
	{"posit16_1f", FACTORYPOSITF(int16_t, 16, 1, float, false) },
	{"posit32_2f", FACTORYPOSITF(int32_t, 32, 2, double, false) },

	// po     sit
	//{"posit16_2", FACTORYPOSIT(int16_t, 16, POSIT16_ES, uint32_t, false) },
	{"posit8_2", FACTORYPOSIT(int8_t, 8, POSIT8_ES, uint32_t, false) },
	{"posit12_2", FACTORYPOSIT(int16_t, 12, POSIT12_ES, uint32_t, false) },

	// tabulated
	{"posit8tab", FACTORY(posit8) } ,
	{"posit12tab", FACTORY(posit12) } ,
	{"" }
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
};
