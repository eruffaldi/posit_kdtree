#pragma once
#include <string>
#include <list>
#include <memory>
#include <iostream>

// TODO better name for init
// TODO add save load tree
// TODO add init from native
class kdtree_any_float
{
public:
	using IndexType = size_t;
	
     virtual ~kdtree_any_float() { std::cerr << "DESTROYED kdtree_any_float " << this << std::endl; };

     virtual int itemsize() const = 0; // name of type

     virtual int itemalign() const = 0; // name of type

     virtual std::string name() const = 0; // name of type

     /// iniializes the tree using float data rows x dim 
     virtual bool initFromFloatTree(const float * data, int rows, int dim, int maxleaf) = 0;

     /// initializes the tree using float data rows x dim 
     virtual bool init(const float * data, int rows, int dim, int maxleaf) = 0;

     /// builds the tree based on the data provided with the init command
     virtual bool build() = 0;

     virtual void dumpData() = 0;

     /// returns up to K neighbors of the point expressed in float and the index output
     virtual int knnSearch(int K, const float * point, IndexType * output) const = 0;

     /// performs a readius search with given radius centered at point up to num_results
     virtual int radiusSearch(float search_radius, const float * point,  int num_results, IndexType * output) const = 0;

     /// stats
     virtual void printStats() const = 0;

     /// emits limits: min,max,eps
     virtual void limits(double out[3]) const = 0;

};


/// creates using name
kdtree_any_float * kdtree_any_float_create(const char * name);

/// list possible nams of the factory above
std::list<std::string> kdtree_any_float_list();
