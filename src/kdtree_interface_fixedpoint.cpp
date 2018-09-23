#include "kdtree_interface_impl.hpp"
#include <limits>
#include <sg14/fixed_point>

#define FIXEDFACTORY(T,frac) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<sg14::fixed_point\
<T,frac> ,nanoflann::L2_Adaptor>(); }) 

/* MINE
namespace std{
template <class T, int N> class numeric_limits<sg14::fixed_point<T,N> > {
public:
  static constexpr bool is_specialized = true;
  static constexpr T min() noexcept { return epsilon(); }
  static constexpr T max() noexcept { return (int64_t(1) << sizeof(T)*8)/(1 << -N); }
  static constexpr T epsilon() noexcept { return 1.0/(1 << -N); } // constant

  static constexpr bool is_signed = true;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = false;
  static constexpr int radix = 2;
};

}
*/

 kdd_factory_t fixedpoint_factories[] = {
	{"fixed32_16", FIXEDFACTORY(int32_t,-16) },
	{"fixed16_8", FIXEDFACTORY(int16_t,-8) },
	{""}
};


//kdtree_any_list qsoftfloat(factories,sizeof(factories)/sizeof(factories[0]));