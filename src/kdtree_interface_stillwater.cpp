#include "kdtree_interface_impl.hpp"
//#define POSIT_THROW_ARITHMETIC_EXCEPTION 1
#include <posit/posit>


using positsc8_0=sw::unum::posit<8, 0>;
using positsc16_1=sw::unum::posit<16, 1>;
using positsc32_2=sw::unum::posit<32, 2>;

/*
Requires
	explicit operator double() const { return toDouble(); }

	explicit operator float() const { return toDouble(); }
*/

kdd_factory_t stillwater_factories[] = {
	// tabulated
	{"posit16_1.still", FACTORY(positsc16_1) } ,
	{"posit32_2.still", FACTORY(positsc32_2) } ,
	{"" }
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
};
