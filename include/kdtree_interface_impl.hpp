#pragma once
#include "kdtree_interface.hpp"
#include "nanoflann.hpp"
#include "nanoflann_utils.hpp"
#include "nanoflann_castclone.hpp"
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>
#include <functional>
#include <type_traits>
#include <limits>


// TODO: Jaccard as in ann-benchmarks
//	 Jaccard = 0 if any length is 0
//
//   intersect = len(a & b)
//	 Jaccard = intersect / (float)(len(a) + len(b) - intersect)

/** Not Normalized Hamming distance functor (generic version, optimized for high-dimensionality data sets).
  * \tparam T Type of the elements (e.g. double, float, uint8_t)
  * \tparam _DistanceType Type of distance variables (can be unsigned) (e.g. float, double, int64_t)
  *
  * as in scipy pdist https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.distance.pdist.html
  * Computes the normalized Hamming distance, or the proportion of those vector elements between two n-vectors u and v which disagree. To save memory, the matrix X can be of type boolean.
  */
template<class T, class DataSource, typename _DistanceType = T>
struct Hamming_Adaptor
{
	typedef T ElementType;
	typedef _DistanceType DistanceType;

	const DataSource &data_source;

	Hamming_Adaptor(const DataSource &_data_source) : data_source(_data_source) { }

	inline DistanceType evalMetric(const T* a, const size_t b_idx, size_t size, DistanceType worst_dist = 1) const
	{
		const T* last = a + size;
		const T* lastgroup = last - 3;
		size_t d = 0;
		int result = 0;

		/* Process 4 items with each loop for efficiency. */
		while (a < lastgroup) {
			const bool diff0 = a[0] != data_source.kdtree_get_pt(b_idx,d++);
			const bool diff1 = a[1] != data_source.kdtree_get_pt(b_idx,d++);
			const bool diff2 = a[2] != data_source.kdtree_get_pt(b_idx,d++);
			const bool diff3 = a[3] != data_source.kdtree_get_pt(b_idx,d++);
			result += diff0 + diff1 + diff2 + diff3;
			a += 4;
		}
		/* Process last 0-3 components.  Not needed for standard vector lengths. */
		while (a < last) {
			result += *a++ != data_source.kdtree_get_pt(b_idx,d++);
		}
		return DistanceType(result);
	}

	template <typename U, typename V>
	inline DistanceType accum_dist(const U a, const V b, int ) const
	{
		return a == b ? DistanceType(0) : DistanceType(1);
	}
};

#if 0

/** Squared Euclidean distance functor (generic version, optimized for high-dimensionality data sets).
  *  Corresponding distance traits: nanoflann::metric_L2
  * \tparam T Type of the elements (e.g. double, float, uint8_t)
  * \tparam _DistanceType Type of distance variables (must be signed) (e.g. float, double, int64_t)
  *
  * 1 - (u dot v) / ||u|| ||v||
  */
template<class T, class DataSource, typename _DistanceType = T>
struct Angular_Adaptor
{
	typedef T ElementType;
	typedef _DistanceType DistanceType;

	const DataSource &data_source;

	Angular_Adaptor(const DataSource &_data_source) : data_source(_data_source) { }


	inline DistanceType evalMetric(const T* a, const size_t b_idx, size_t size) const
	{
		DistanceType dotaccum(0);
		DistanceType vaccum(0);
		DistanceType uaccum(0);

		for (size_t i = 0; i < size; ++i) {
			auto x = data_source.kdtree_get_pt(b_idx, i);
			uaccum += square(a[i]);
			vaccum += square(x);
			dotaccum += a[i]*x;
		}
		return DistanceType(1)-(dotaccum/(std::sqrt(uaccum)*std::sqrt(vaccum)));
	}

	template <typename U, typename V>
	inline DistanceType accum_dist(const U a, const V b, int ) const
	{
		// TODO CLARIFY
		return square(a - b);
	}

};
#endif


template <typename T, typename IndexType>
struct NDDataHolder
{
	int stride,rows,dims;
	std::vector<T>  pts; // matrix

	NDDataHolder(int arows, int adims, int astride): rows(arows),pts(arows*astride),dims(adims),stride(astride)
	{
		std::cout << "allocating " << rows << " " << dims << "  to " << pts.size() << " " << typeid(T).name() << std::endl;
	}


	// Must return the number of data points
	inline IndexType kdtree_get_point_count() const { return rows; }

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline T kdtree_get_pt(const IndexType irow, int idim) const
	{
		return irow >= rows || idim < 0 || idim >= dims ? T() : pts[irow*stride+idim];
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

/**
 * std::copy but forcing a cast to decltype of *d
 */
template <class SrcIt, class DstIt>
DstIt castcopy(SrcIt f, SrcIt e, DstIt d)
{
	for(; f != e; )
		*d++ = typename std::iterator_traits<DstIt>::value_type(*f++);
	return d;
}

template <class T, template<class XT, class DataSource, typename _DistanceType = XT> class Adaptor>
class kdtree_any_float_impl:  public kdtree_any_float
{
public:
	using num_t = T;

	/*
	typename Distance, 
	class DatasetAdaptor, 
	int DIM = -1,
    typename IndexType = size_t
    */

	// construct a kd-tree index:
	typedef nanoflann::KDTreeSingleIndexAdaptor<
		Adaptor<num_t, NDDataHolder<num_t,IndexType> > ,
		NDDataHolder<num_t,IndexType>, -1, IndexType
		> my_kd_tree_t;

	// construct a kd-tree index:
	typedef nanoflann::KDTreeSingleIndexAdaptor<
		Adaptor<float, NDDataHolder<float,IndexType> > ,
		NDDataHolder<float,IndexType>, -1, IndexType
		> my_kd_tree_float_t;

     virtual ~kdtree_any_float_impl() 
     {

     }

     virtual int limits(double *out, uint64_t *rout, int n) const 
     {
     	const int r = 5;
     	if(n != r || !out || !rout)
     		return r;
     	T v;
     	int i = 0;
     	v = std::numeric_limits<T>::max();
     	out[i++] = (double)v;
     	v = std::numeric_limits<T>::min();
     	out[i++] = (double)v;
     	v = std::numeric_limits<T>::epsilon();
     	out[i++] = (double)v;
     	for(int i = 0; i < n; i++)
     		rout[i] = 0;
     	return r;
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

     virtual bool initFromFloatTree(const float * data, int rows, int dim, int maxleaf) 
     {
     	if(!data || rows < 0 || dim <= 0)
     		return false;
     	if(maxleaf <= 0)
     		maxleaf = 1;
     	dim_ = dim;
     	cloud_f = std::make_shared<NDDataHolder<float,IndexType>> (rows,dim,dim);
     	cloud = std::make_shared<NDDataHolder<num_t,IndexType> > (rows,dim,dim);
     	index.reset();
     	index_f.reset();
     	if(!cloud_f || !cloud)
     	{
     		return false;
     	}
     	else
     	{
	     	castcopy(data,data+rows*dim,&cloud->pts[0]);
	     	castcopy(data,data+rows*dim,&cloud_f->pts[0]);
	     	index_f = std::make_shared<my_kd_tree_float_t>(dim_, *cloud_f, nanoflann::KDTreeSingleIndexAdaptorParams(maxleaf /* max leaf */) );
	     	index = std::make_shared<my_kd_tree_t>(dim_, *cloud, nanoflann::KDTreeSingleIndexAdaptorParams(maxleaf /* max leaf */) );
	     	return (bool)index_f && (bool)index;
	     }
     }

     virtual void dumpData()
     {
     	if(!cloud)
     	{
     		std::cout << "missing cloud\n";
     		return;
     	}
     	int s = cloud->stride;
     	std::cout << "<<dumping " << typeid(*this).name() << " rows " << cloud->rows << " dims " << cloud->dims << std::endl;
     	for(int i = 0; i < cloud->rows; i++)
     	{
     		for(int j = 0; j < cloud->dims; j++)
     			std::cout << cloud->pts[i*s+j] << " ";
     		std::cout << std::endl;
     	}
     	std::cout << "dumping>>\n";
     }


     virtual bool init(const float * data, int rows, int dim, int maxleaf) 
     {
     	if(!data || rows < 0 || dim <= 0)
     		return false;
     	if(maxleaf <= 0)
     		maxleaf = 1;
     	dim_ = dim;
     	cloud_f.reset();
     	index_f.reset();
     	index.reset();
     	cloud = std::make_shared<NDDataHolder<num_t,IndexType>> (rows,dim,dim);
     	if(!cloud)
     	{
     		return false;
     	}
     	else
     	{
	     	castcopy(data,data+rows*dim,&cloud->pts[0]);
	     	index = std::make_shared<my_kd_tree_t>(dim_, *cloud, nanoflann::KDTreeSingleIndexAdaptorParams(maxleaf /* max leaf */) );
	     	return (bool)index;
	     }
     }

     /*
     template <class D, class DN, class SN>
     void castcopytree(D*d, DN&dn, const SN&sn)
     {
 		using DT = typename my_kd_tree_t::DistanceType;
		if(sn->child1 != 0 && sn->child2 != 0) // is_leaf
		{
			const auto & smn = *sn;
			auto *mn = d->pool.template allocate<typename D::Node>();
			mn->node_type.sub.divfeat = smn.node_type.sub.divfeat;
			mn->node_type.sub.divlow = DT(smn.node_type.sub.divlow); // casting
			mn->node_type.sub.divhigh = DT(smn.node_type.sub.divhigh); // casting
			dn=mn;
			if(smn->child1)
				castcopytree(d, mn->child1, smn.child1);
			else
				mn->child1 =0;
			if(smn->child2)
				castcopytree(d, mn->child2, smn.child2);
			else
				mn->child2 = 0;
		}
		else
		{
			int div = sn->node_type.sub.divfeat;
			const auto & sln = *sn;
			auto *ln = d->pool.template allocate<typename D::Node >();
			dn=ln;
			ln->node_type.lr.left = sln.node_type.lr.left;
			ln->node_type.lr.right = sln.node_type.lr.right;
		}     	
     }
     */


     virtual bool build()
     {
     	if(index_f && index)
     	{
		    typename my_kd_tree_float_t::BoundingBox bb;
		    index_f->computeBoundingBox(bb);
     		index_f->buildIndex();
     		castcopyindex(index_f.get(),index.get());     		
     		return true;
     	}
     	else if(index)
     	{
		    typename my_kd_tree_t::BoundingBox bb;
		    index->computeBoundingBox(bb);
		 	index->buildIndex();
     		return true;
     	}
     	else
     	{
     		return false;
     	}
     }


     virtual int knnSearch(int num_results, const float * query_point_f,IndexType * output)  const
     {
     	if(!index || num_results < 1 || query_point_f == 0 || output == 0)
     		return 0;
		std::vector<num_t> query_point(dim_);
		castcopy(query_point_f,query_point_f+dim_,query_point.begin());

		std::vector<num_t> out_dist_sqr(num_results);
		return index->knnSearch(&query_point[0], num_results, output, &out_dist_sqr[0]);
     }

     virtual int radiusSearch(float asearch_radius, const float * query_point_f,  int num_results, IndexType * output)  const
     {
     	if(!index || num_results < 1 || query_point_f == 0 || output == 0 || !(asearch_radius > 0))
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
     std::shared_ptr<my_kd_tree_float_t> index_f;
	 std::shared_ptr<NDDataHolder<float,IndexType> > cloud_f;
     int dim_ = 0;
};

#if 0
// previous version based on registrar
struct kdtree_any_list
{
public:
	// MAKE this template ctor
	kdtree_any_list(kdd_factory_t *p, int N): begin_(p),end_(p+N),next(top)
	{
		std::cout << "adding stuff: " << this << top << next << std::endl;
		top = this;
	}
	kdd_factory_t * begin() const { return begin_; }
	kdd_factory_t * end() const { return end_; }

	kdd_factory_t * begin_, * end_;
	kdtree_any_list * next = 0;

	static kdtree_any_list * top;
};
#endif

#define FACTORY(T) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<T,nanoflann::L2_Adaptor>(); }) 

#define FACTORYH(T) std::function<kdtree_any_float*()>([] () { return new kdtree_any_float_impl<T,Hamming_Adaptor>(); }) 
