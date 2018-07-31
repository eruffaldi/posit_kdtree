#!/bin/bash
#
# docker run 
git clone --depth 1 https://github.com/eruffaldi/cppPosit.git
cd cppPosit
mkdir build
cd build
cmake ..
make install
