#include "kdtree_interface_impl.hpp"

//kdtree_any_list * kdtree_any_list::top = 0;

//kdd_factory_t __attribute__((weak)) cppPosit_factories[] = {0};
//kdd_factory_t __attribute__((weak)) native_factories[] = {0};
kdd_factory_t __attribute__((weak)) cppPosit_factories[] = { {""} };
kdd_factory_t __attribute__((weak)) softfloat_factories[] = { {""} };
kdd_factory_t __attribute__((weak)) native_factories[] = { {""} };

static kdd_factory_t * factories[]
{
	cppPosit_factories,
	softfloat_factories,
	native_factories
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

