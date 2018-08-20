
#include <cstdint>
#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/numpy.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "kdtree_interface.hpp"

namespace py = boost::python;
namespace np = boost::python::numpy;

class kdtree_any_float_wrap
{
public:
    kdtree_any_float_wrap(std::string name): p_(kdtree_any_float_create(name.c_str()))
    {
        if(!p_)
            std::cerr << "kdtree_any_float_wrap: not found " << name << std::endl;
        //else
        //  std::cerr << "kdtree_any_float_wrap: init " << name << " " << p_->name()<< std::endl;
    }

    ~kdtree_any_float_wrap()
    {
        //std::cerr << "kdtree_any_float_wrap: dtor" << std::endl;
        delete p_;
    }   

    #if 0
    bool buildx(np::ndarray data, int maxleaf)
    {
        if(data.get_dtype() != np::dtype::get_builtin<float>())
            return
        return build((float*)data.get_data(),data.shape(0),data.shape(1),maxleaf);
    }
    #endif
    bool buildx(std::intptr_t pdata, int rows, int dim, int maxleaf, bool fromfloatree)
    {
        return build(reinterpret_cast<const float*>(pdata),rows,dim,maxleaf,fromfloatree);
    }

    bool build(const float* pdata, int rows, int dim, int maxleaf, bool fromfloatree)
    {
        if(!p_) 
            return false;
        else
        {
            /*
            for(int i = 0; i < rows; i++)
            {
                for(int j = 0; j < dim; j++)
                {
                    std::cout  << " " << pdata[i*dim+j];
                }
                std::cout << std::endl;
            }
            */
            if(!fromfloatree)
                return p_->init(pdata,rows,dim,maxleaf) && p_->build();
            else
                return p_->initFromFloatTree(pdata,rows,dim,maxleaf) && p_->build();

        }
    }

    void printStats() const
    {
        if(!p_)
            std::cout<<"not assigned\n";
        else
        {
            p_->printStats();
        }
    }

    int knnSearch(int K, const float* point, kdtree_any_float::IndexType * output) const 
    {
        if(!p_) 
            return 0;
        else
            return p_->knnSearch(K,point,output);
    }

    int knnSearchx(int K, std::intptr_t  point, std::intptr_t output) const 
    {
        return knnSearch(K,reinterpret_cast<const float*>(point),reinterpret_cast<kdtree_any_float::IndexType*>(output));
    }

    int radiusSearch(float search_radius, const float * point,  int num_results, kdtree_any_float::IndexType * output) const 
    {
        if(!p_) 
            return 0;
        else
            return p_->radiusSearch(search_radius,point,num_results,output);
    }

    int radiusSearchx(float search_radius, std::intptr_t point,  int num_results, std::intptr_t output) const 
    {
        if(!p_) 
            return 0;
        else
            return radiusSearch(search_radius,reinterpret_cast<const float*>(point),num_results,reinterpret_cast<kdtree_any_float::IndexType*>(output));
    }

    int itemsize() const
    {
        return p_ ? p_->itemsize() : 0;
    }

     int itemalign() const
     {
        return p_ ? p_->itemalign() : 0;        
     }

     int indexsize() const
     {
        return sizeof(kdtree_any_float::IndexType);
     }

     std::string name() const 
     {
        return p_ ? p_->name() : "";
     }

     // https://gist.github.com/andreasgrv/1d4f9320552ce9bb6dba
    static    py::list list()
    {
        py::list r;
        for (auto c : kdtree_any_float_list()) {
            r.append(c);
        }
        return r;
    }

    kdtree_any_float * p_;
};

BOOST_PYTHON_MODULE(pynanoflann_any)
{

    py::class_<kdtree_any_float_wrap>("kdtree_any_float",py::init<std::string>())
        .def("build",&kdtree_any_float_wrap::build)
        .def("buildx",&kdtree_any_float_wrap::buildx)
        .def("knnSearch",&kdtree_any_float_wrap::knnSearch)
        .def("knnSearchx",&kdtree_any_float_wrap::knnSearchx)
        .def("radiusSearch",&kdtree_any_float_wrap::radiusSearch)
        .def("radiusSearchx",&kdtree_any_float_wrap::radiusSearchx)
        .def("itemsize",&kdtree_any_float_wrap::itemsize)
        .def("itemalign",&kdtree_any_float_wrap::itemalign)
        .def("indexsize",&kdtree_any_float_wrap::indexsize)
        .def("name",&kdtree_any_float_wrap::name)
        .def("printStats",&kdtree_any_float_wrap::printStats)

        .def("list", &kdtree_any_float_wrap::list)
        .staticmethod("list")
    ;
};