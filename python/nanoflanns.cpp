
#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <string>
#include <sstream>
#include <vector>
#include "nanoflann.hpp"
#include "nanoflann_utils.hpp"


using namespace boost::python;
using namespace nanoflann;

// construct a kd-tree index:
#define num_t float
class KUse
{
public:
    KUse(int dim, int n, int64_t points, int maxleaf)
    : KUse(dim,n,(float*)points,maxleaf)
    {
    }

    KUse(int dim, int n, float * points, int maxleaf)
    : cloud(),index(std::make_shared<index_t>(dim,cloud,KDTreeSingleIndexAdaptorParams(maxleaf)))
    {
        int np = n/3;
        cloud.pts.resize(np);
        for(int i = 0, k = 0; i < np; k += 3,i++)
        {
            auto & p = cloud.pts[i];
            p.p.x = points[k+0];
            p.p.y = points[k+1];
            p.p.z = points[k+2];
        }
        index->computeBoundingBox(bb);
        native_query_pt.resize(dim);
    }

    int knnSearchx(int dim, int64_t query_pt, int num_results, int64_t  indices)
    {
        return knnSearch(dim,(float*)query_pt,num_results,(int*)indices);
    }

    int knnSearch(int dim, float * query_pt, int num_results, int * indices)
    {
        castquerypoint(dim,query_pt,native_query_pt);
        ret_index.resize(num_results);   
        out_dist_sqr.resize(num_results);
        num_results = index->knnSearch(&native_query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);

        std::copy(ret_index.begin(),ret_index.begin()+num_results,indices);
        return num_results;
    }

    int radiusSearch(int dim, float * query_pt, float search_radius, int num_results, int * indices)
    {
        nanoflann::SearchParams params;
        ret_matches.clear();
        castquerypoint(dim,query_pt,native_query_pt);
        const size_t nMatches = index->radiusSearch(&native_query_pt[0], (num_t)search_radius, ret_matches, params);        
        int amin = num_results < nMatches ? num_results : nMatches;
        for(int i = 0; i < amin; i++)
            indices[i] = ret_matches[i].first;
        return amin;
    }

    void castquerypoint(int dim, float * query_pt, std::vector<num_t> & native_query_pt)
    {
        int amin = dim < index->dim ? dim: index->dim;
        for(int i = 0; i < amin; i++)
            native_query_pt[i] = query_pt[i]; // float to num_t
        for(int i = amin; i < native_query_pt.size(); i++)
            native_query_pt[i] = 0; // num_t zero
    } 

    void buildIndex()
    {
        index->buildIndex();
       // std::cout << "index memory:" << index->usedMemory(index) << std::endl;
       // std::cout << "points: " << index->m_size << " dim:" <<  index->dim << " indice:" << index->vind.size() << " m_leaf_max_size:" << index->m_leaf_max_size << std::endl;
    }

    PointCloud<num_t> cloud;

    typedef KDTreeSingleIndexAdaptor<
        L2_Simple_Adaptor<num_t, PointCloud<num_t> > ,
        PointCloud<num_t>,
        3 /* dim */
        > index_t;

    std::shared_ptr<index_t> index;
    typename index_t::BoundingBox bb;
    std::vector<size_t>   ret_index;
    std::vector<num_t> out_dist_sqr;
    std::vector<num_t> native_query_pt;
    std::vector<std::pair<size_t,num_t> >   ret_matches;
};


BOOST_PYTHON_MODULE(nanoflanns)
{
    class_<KUse>("KUse",init<int,int,int64_t,int>())
        .def("buildIndex",&KUse::buildIndex)
        .def("knnSearch",&KUse::knnSearch)
        .def("knnSearchx",&KUse::knnSearchx)
        .def("radiusSearch",&KUse::radiusSearch)
    ;
};