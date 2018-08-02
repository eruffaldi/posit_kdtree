#!/bin/bash
#
# docker run 
git clone https://$1:$2@bitbucket.org/eruffaldi/posit_kdtree.git
cd posit_kdtree
mkdir build
cd build
cmake -DPython_ADDITIONAL_VERSIONS=3.7 -DPYTHON_LIBRARIES="/usr/lib/libpython37.dylib" -DPYTHON_INCLUDE_DIRS="/usr/include/python/3.7" -DPYTHON_EXECUTABLE="/usr/bin/python3" -DPYTHON_VERSION_SUFFIX=3 -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
make
