
#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "kdtree_interface.hpp"

namespace py = boost::python;



class kdtree_any_float_wrap
{
public:
    kdtree_any_float_wrap(std::string name): p_(kdtree_any_float_create(name.c_str()))
    {
        if(!p_)
            std::cerr << "kdtree_any_float_wrap: not found " << name << std::endl;
        else
            std::cerr << "kdtree_any_float_wrap: init " << name << " " << p_->name()<< std::endl;
    }

    ~kdtree_any_float_wrap()
    {
        std::cerr << "kdtree_any_float_wrap: dtor" << std::endl;
        delete p_;
    }

    bool buildx(int64_t pdata, int rows, int dim, int maxleaf)
    {
        return build((float*)pdata,rows,dim,maxleaf);
    }

    bool build(float* pdata, int rows, int dim, int maxleaf)
    {
        if(!p_) 
            return false;
        return p_->init(pdata,rows,dim,maxleaf) && p_->build();
    }

    int knnSearch(int K, float * point, size_t * output) const 
    {
        if(!p_) 
            return 0;
        return p_->knnSearch(K,point,output);
    }

    int knnSearchx(int K, int64_t  point, int64_t output) const 
    {
        return knnSearch(K,(float*)point,(size_t*)output);
    }

    int radiusSearch(float search_radius, float * point,  int num_results, int * output) const 
    {
        if(!p_) 
            return 0;
        return p_->radiusSearch(search_radius,point,num_results,output);
    }

    int radiusSearchx(float search_radius, int64_t point,  int num_results, int64_t output) const 
    {
        if(!p_) 
            return 0;
        return radiusSearch(search_radius,(float*)point,num_results,(int*)output);
    }

    int itemsize() const
    {
        return p_ ? p_->itemsize() : 0;
    }

     int itemalign() const
     {
        return p_ ? p_->itemalign() : 0;        
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

BOOST_PYTHON_MODULE(nanoflanns2)
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
        .def("name",&kdtree_any_float_wrap::name)

        .def("list", &kdtree_any_float_wrap::list)
        .staticmethod("list")
    ;
};