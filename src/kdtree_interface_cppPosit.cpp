#include "kdtree_interface_impl.hpp"
#include "posit.h"
#include "posit12.hpp"
//#include "posit16.hpp"
#include "posit10.hpp"
#include "posit8.hpp"

kdd_factory_t cppPosit_factories[] = {
	// po     sit
	//{"posit16_2", FACTORYPOSIT(int16_t, 16, POSIT16_ES, uint32_t, false) },
	{"posit12_2", FACTORYPOSIT(int16_t, 12, POSIT12_ES, uint32_t, false) },
	{"posit8_2", FACTORYPOSIT(int8_t, 8, POSIT8_ES, uint32_t, false) },
	// tabulated
	{"posit8tab", FACTORY(posit8) } ,
	{"posit12tab", FACTORY(posit12) } ,
	{"" }
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
};
