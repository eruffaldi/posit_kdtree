    
#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
struct World
{
    World(const char *x): mMsg(x){}
    void set(std::string msg) { mMsg = msg; }
    void many(boost::python::list msgs) {
        long l = len(msgs);
        std::stringstream ss;
        for (long i = 0; i<l; ++i) {
            if (i>0) ss << ", ";
            std::string s = boost::python::extract<std::string>(msgs[i]);
            ss << s;
        }
        mMsg = ss.str();
    }
    float passdata(int64_t px, int rows, int cols)
    {
        float * p = (float*)px;
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++)
            {
                std::cout << i << " " << j << " " << p[i*cols+j] << std::endl;
            }
        return 100.5;
    }
    std::string greet() { return mMsg; }
    std::string mMsg;
};

using namespace boost::python;

BOOST_PYTHON_MODULE(extest)
{
    class_<World>("World",init<const char*>(args("name")))
        .def("greet", &World::greet)
        .def("set", &World::set)
        .def("many", &World::many)
        .def("passdata", &World::passdata)
    ;
};