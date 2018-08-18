#!/bin/bash
#
# docker run 
git clone --depth 1 https://gitlab.com/cerlane/SoftPosit
cd SoftPosit
cd build/Linux-x86_64-GCC
make	
cp softposit.a /usr/lib/libsoftposit.a
cp ../../source/include/softposit.h ../../source/include/softposit_cpp.h ../../source/include/softposit_types.h /usr/include