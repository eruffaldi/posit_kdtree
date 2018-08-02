#!/bin/bash
#
# docker run 
git clone --depth 1 https://github.com/eruffaldi/berkeley-softfloat-3.git
cd berkeley-softfloat-3
mkdir build
cd build
cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
make install
