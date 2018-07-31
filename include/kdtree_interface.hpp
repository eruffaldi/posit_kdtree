#pragma once
#include <string>
#include <list>

class kdtree_any_float
{
public:
	using IndexType = size_t;
	
     virtual ~kdtree_any_float() {};
     virtual int itemsize() const = 0; // name of type
     virtual int itemalign() const = 0; // name of type
     virtual std::string name() const = 0; // name of type
     virtual bool init(const float * data, int rows, int dim, int maxleaf) = 0;
     virtual bool build() = 0;
     virtual int knnSearch(int K, const float * point, IndexType * output) const = 0;
     virtual int radiusSearch(float search_radius, const float * point,  int num_results, IndexType * output) const = 0;
     virtual void printStats() const = 0;

};


kdtree_any_float * kdtree_any_float_create(const char * name);

std::list<std::string> kdtree_any_float_list();
