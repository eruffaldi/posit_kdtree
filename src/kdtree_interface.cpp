#include "kdtree_interface_impl.hpp"

/*
Alternatively we could use weak

kdd_factory_t __attribute__((weak)) cppPosit_factories[] = { {""} };
kdd_factory_t __attribute__((weak)) softfloat_factories[] = { {""} };
kdd_factory_t __attribute__((weak)) native_factories[] = { {""} };
kdd_factory_t __attribute__((weak)) softposit_factories[] = { {""} };
*/
extern kdd_factory_t cppPosit_factories[] ;
extern kdd_factory_t softfloat_factories[] ;
extern kdd_factory_t native_factories[] ;
extern kdd_factory_t  softposit_factories[];
extern kdd_factory_t fixedpoint_factories[];

static kdd_factory_t * factories[]
{
	cppPosit_factories,
	softfloat_factories,
	native_factories,
	softposit_factories,
	fixedpoint_factories
};

kdtree_any_float * kdtree_any_float_create(const char * name)
{
	for(kdd_factory_t * p : factories)
	{
		if(!p)
			break;
		for(kdd_factory_t * f = p; !f->name.empty(); f++)
		{
			if(f->name == name)
			{
				auto *p= f->fx();
				return p;
			}
		}
	}
	return 0;
}

std::list<std::string> kdtree_any_float_list()
{
	std::list<std::string> r;
	for(kdd_factory_t * p : factories)
	{
		if(!p)
			break;
		for(kdd_factory_t * f = p; !f->name.empty(); f++)
		{
			r.push_back(f->name);
		}
	}
	return r;
}

