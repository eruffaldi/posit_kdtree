#!/bin/bash
#
# docker run 
git clone --depth 1 https://github.com/eruffaldi/cppPosit.git
cd cppPosit
mkdir build
cd build
cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
make install
