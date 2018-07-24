#include "kdtree_interface.hpp"
#include <nanoflann.hpp>
#include "nanoflann_utils.hpp"
#include "posit.h"
#include "posit12.hpp"
#include "posit10.hpp"
#include "posit8.hpp"
#include "softfloat.hpp"
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>

template <typename T>
struct NDDataHolder
{
	int stride,rows,dim;
	std::vector<T>  pts; // matrix

	NDDataHolder(int arows, int adim, int astride): rows(arows),pts(rows*stride),dim(adim),stride(astride)
	{
	}

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return pts.size(); }

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline T kdtree_get_pt(const size_t idx, int dim) const
	{
		return pts[idx*stride+dim];
	}

	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }

};

struct kdd_factory_t {
	std::string name;
	std::function<kdtree_any_float*()> fx;
};

template <class It, class Dst>
Dst castcopy(It f, It e, Dst d)
{
	while(f != e)
		*d++ = (decltype(*d))*f++;
	return d;
}

template <class T>
class kdtree_any_float_impl
{
public:
	using num_t = T;

	// construct a kd-tree index:
	typedef nanoflann::KDTreeSingleIndexAdaptor<
		nanoflann::L2_Simple_Adaptor<num_t, NDDataHolder<num_t> > ,
		NDDataHolder<num_t>, -1
		> my_kd_tree_t;

     virtual ~kdtree_any_float_impl() 
     {

     }

     virtual std::string name() { return typeid(T).name(); } 

     virtual bool init(float * data, int rows, int dim, int maxleaf) 
     {
     	dim_ = dim;
     	cloud = std::make_shared<NDDataHolder<num_t>> (rows,dim,dim);
     	if(!cloud)
     		return false;
     	castcopy(data,data+rows*dim,cloud->pts.begin());
     	index = std::make_shared<my_kd_tree_t>(dim_, *cloud, nanoflann::KDTreeSingleIndexAdaptorParams(maxleaf /* max leaf */) );
     	return (bool)index;
     }
     virtual bool build()
     {
     	if(!index)
     		return false;
     	else
     	{
		    typename my_kd_tree_t::BoundingBox bb;
		    index->computeBoundingBox(bb);
     		index->buildIndex();
     		return true;
     	}
     }

     virtual int knnSearch(int K, float * query_point_f, int num_results, size_t * output)  const
     {
     	if(!index)
     		return 0;
		std::vector<num_t> query_point(num_results);
		castcopy(query_point_f,query_point_f+dim_,query_point.begin());

		std::vector<size_t>   ret_index(num_results);
		std::vector<num_t> out_dist_sqr(num_results);
		num_results = index->knnSearch(&query_point[0], num_results, &ret_index[0], &out_dist_sqr[0]);
		castcopy(ret_index.begin(),ret_index.begin()+num_results,output);
		return num_results;
     }

     virtual int radiusSearch(float asearch_radius, float * query_point_f,  int num_results, int * output)  const
     {
     	if(!index)
     		return 0;
		const num_t search_radius = static_cast<num_t>(asearch_radius);
		std::vector<std::pair<size_t,num_t> >   ret_matches;
		nanoflann::SearchParams params;
		std::vector<num_t> query_point(num_results);
		castcopy(query_point_f,query_point_f+dim_,query_point.begin());
		//params.sorted = false;
		const size_t nMatches = index->radiusSearch(&query_point[0], search_radius, ret_matches, params);	
		for(auto & x: ret_matches)
			*output++ = x.first;
		return nMatches;
     }

     virtual int itemsize() const { return sizeof(T); }
     virtual int itemalign()const { return alignof(T); }

	 std::shared_ptr<NDDataHolder<T> > cloud;
     std::shared_ptr<my_kd_tree_t> index;
     int dim_;
};

#define FACTORY(T) [] () { return (kdtree_any_float*)new kdtree_any_float_impl<T>(); } 

static kdd_factory_t factories[] = {
	{"float", FACTORY(float) } ,
	{"double", FACTORY(double) } ,
	{"posit8", FACTORY(posit8) } ,
	{"posit10", FACTORY(posit10) },
	{"soft16", FACTORY(softfloat16) }
};

kdtree_any_float * kdtree_any_float_create(const char * name)
{
	for(auto & f: factories)
	{
		if(f.name == name)
		{
			std::cerr << "found and ctor" << std::endl;
			return f.fx();
		}
	}
	return nullptr;
}

std::list<std::string> kdtree_any_float_list()
{
	std::list<std::string> r;
	for(auto & f: factories)
		r.push_back(f.name);
	return r;
}
