#include "kdtree_interface.hpp"
#include "nanoflann.hpp"
#include "nanoflann_utils.hpp"
#include "posit.h"
#include "posit12.hpp"
//#include "posit16.hpp"
#include "posit10.hpp"
#include "posit8.hpp"
#include "softfloat.hpp"
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>
#include <functional>

template <typename T, typename IndexType>
struct NDDataHolder
{
	int stride,rows,dim;
	std::vector<T>  pts; // matrix

	NDDataHolder(int arows, int adim, int astride): rows(arows),pts(rows*stride),dim(adim),stride(astride)
	{
	}

	// Must return the number of data points
	inline IndexType kdtree_get_point_count() const { return rows; }

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline T kdtree_get_pt(const IndexType idx, int dim) const
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
	for(; f != e; f++, d++)
		*d = (decltype(*d))*f;
	return d;
}

template <class T>
class kdtree_any_float_impl:  public kdtree_any_float
{
public:
	using num_t = T;

	// construct a kd-tree index:
	typedef nanoflann::KDTreeSingleIndexAdaptor<
		nanoflann::L2_Simple_Adaptor<num_t, NDDataHolder<num_t,IndexType> > ,
		NDDataHolder<num_t,IndexType>, -1, IndexType
		> my_kd_tree_t;

     virtual ~kdtree_any_float_impl() 
     {

     }

     virtual void printStats() const
     {
     	if(!index)
     		std::cout << "not inited\n";
     	else
     	{
	     	std::cout << "index memory:" << index->usedMemory(*index) << std::endl;
			std::cout << "points: " << index->m_size << " dim:" <<  index->dim << " indice:" << index->vind.size() << " m_leaf_max_size:" << index->m_leaf_max_size << std::endl;
			std::cout << "EPS 1e-5 " << (float)(static_cast<T>(0.00001)) << " and 1-eps" << (float)(T(1) - T(0.00001)) << std::endl;
		}	
     }

     virtual std::string name() const { return typeid(T).name(); } 

     virtual bool init(const float * data, int rows, int dim, int maxleaf) 
     {
     	dim_ = dim;
     	cloud = std::make_shared<NDDataHolder<num_t,IndexType>> (rows,dim,dim);
     	if(!cloud)
     		return false;
     	else
     	{
	     	castcopy(data,data+rows*dim,cloud->pts.begin());
	     	index = std::make_shared<my_kd_tree_t>(dim_, *cloud, nanoflann::KDTreeSingleIndexAdaptorParams(maxleaf /* max leaf */) );
	     	return (bool)index;
	     }
     }
     virtual bool build()
     {
     	if(!index)
     	{
     		return false;
     	}
     	else
     	{
		    typename my_kd_tree_t::BoundingBox bb;
		    index->computeBoundingBox(bb);
     		index->buildIndex();
     		return true;
     	}
     }

     virtual int knnSearch(int K, const float * query_point_f,IndexType * output)  const
     {
     	if(!index)
     		return 0;
     	auto num_results = K;
		std::vector<num_t> query_point(num_results);
		castcopy(query_point_f,query_point_f+dim_,query_point.begin());

		// is necessary out_dist_sqr ?
		std::vector<num_t> out_dist_sqr(K);
		num_results = index->knnSearch(&query_point[0], num_results, output, &out_dist_sqr[0]);
		//std::cout<<"resulted " << num_results << " for " << K << " first " << output[0] << " then " << output[1] << std::endl;
		//castcopy(ret_index.begin(),ret_index.begin()+num_results,output);
		return num_results;
     }

     virtual int radiusSearch(float asearch_radius, const float * query_point_f,  int num_results, IndexType * output)  const
     {
     	if(!index)
     		return 0;
		const num_t search_radius = static_cast<num_t>(asearch_radius);
		std::vector<std::pair<IndexType,num_t> >   ret_matches;
		nanoflann::SearchParams params;
		std::vector<num_t> query_point(num_results);
		castcopy(query_point_f,query_point_f+dim_,query_point.begin());
		//params.sorted = false;
		const IndexType nMatches = index->radiusSearch(&query_point[0], search_radius, ret_matches, params);	
		for(int i = 0; i < nMatches; i++)
			output[i] = ret_matches[i].first;
		return nMatches;
     }

     virtual int itemsize() const { return sizeof(T); }
     virtual int itemalign()const { return alignof(T); }

	 std::shared_ptr<NDDataHolder<T,IndexType> > cloud;
     std::shared_ptr<my_kd_tree_t> index;
     int dim_;
};

#define FACTORY(T) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<T>(); }) 

#define FACTORYPOSIT(stype,bits,esbits,ftype,withnan) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<Posit<stype,bits,esbits,ftype,withnan> >(); }) 

static kdd_factory_t factories[] = {
	// native
	{"float", FACTORY(float) } ,
	{"double", FACTORY(double) } ,
	// posit
	//{"posit16_2", FACTORYPOSIT(int16_t, 16, POSIT16_ES, uint32_t, false) },
	{"posit12_2", FACTORYPOSIT(int16_t, 12, POSIT12_ES, uint32_t, false) },
	{"posit8_2", FACTORYPOSIT(int8_t, 8, POSIT8_ES, uint32_t, false) },
	// tabulated
	{"posit8tab", FACTORY(posit8) } ,
	{"posit12tab", FACTORY(posit12) } ,
	/*{"posit10", FACTORY(posit10) },*/
	// softfloat 16bit ... TODO make it tabulated
	{"soft16", FACTORY(softfloat16) }
};

kdtree_any_float * kdtree_any_float_create(const char * name)
{
	for(auto & f: factories)
	{
		if(f.name == name)
		{
			auto *p= f.fx();
			return p;
		}
	}
	return 0;
}

std::list<std::string> kdtree_any_float_list()
{
	std::list<std::string> r;
	for(auto & f: factories)
		r.push_back(f.name);
	return r;
}
