
#include <cstdint>
#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "kdtree_interface.hpp"

namespace py = boost::python;
//namespace np = boost::python::numpy;

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

    virtual ~kdtree_any_float_wrap()
    {
        std::cerr << "kdtree_any_float_wrap: dtor " << this << std::endl;
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
        if(p_ && (!pdata || rows < 0 || dim < 0 || maxleaf <= 0))
            return false;
        else
        {
            dim_ = dim;
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
        {
            std::cerr << "not assigned\n";
        }
        else
        {
            p_->printStats();
        }
    }

    int knnSearchx(int npoints, int num_results, std::intptr_t  point, std::intptr_t output) const 
    {
        return knnSearch(npoints, num_results,reinterpret_cast<const float*>(point),reinterpret_cast<kdtree_any_float::IndexType*>(output));
    }

    int knnSearch(int npoints, int num_results, const float* point, kdtree_any_float::IndexType * output) const 
    {
        if(!p_ || !point || !output || num_results < 1 || npoints <= 0) 
            return 0;
        else
        {
            return p_->knnSearch(npoints, num_results,point,output);
        }
    }

    int radiusSearch(int npoints,  int num_results, float search_radius, const float * point,  kdtree_any_float::IndexType * output) const 
    {
        if(!p_ || !point || !output || search_radius <= 0 || num_results < 1 || npoints <= 0) 
            return 0;
        else
        {
            return p_->radiusSearch(npoints, num_results, search_radius,point,output);
        }
    }

    int radiusSearchx(int npoints, int num_results , float search_radius, std::intptr_t point,  std::intptr_t output) const 
    {
        return radiusSearch(npoints, num_results, search_radius,reinterpret_cast<const float*>(point),reinterpret_cast<kdtree_any_float::IndexType*>(output));
    }

    int limitssize()
    {
        if(!p_)
            return 0;
        else
            return p_->limits(0,0,0);
    }

    int limitsx(std::intptr_t qd, std::intptr_t qr, int n)
    {
        return !qd || !qr ? 0 : limits(reinterpret_cast<double*>(qd),reinterpret_cast<uint64_t*>(qr),n);
    }

    int limits(double *qd, uint64_t *qr, int n)
    {
        if(!p_) 
            return 0;
        else
        {
            return p_->limits(qd,qr,n);
        }

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

    int dim_ = 0;
    kdtree_any_float * p_ = 0;
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
        .def("limitssize",&kdtree_any_float_wrap::limitssize)
        .def("limitsx",&kdtree_any_float_wrap::limitsx)

        .def("list", &kdtree_any_float_wrap::list)
        .staticmethod("list")
    ;
};
