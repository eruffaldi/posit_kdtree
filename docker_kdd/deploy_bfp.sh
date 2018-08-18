#!/bin/bash
#
# docker run 
git clone --depth 1 https://github.com/libcg/bfp
cd bfp

cp Makefile Makefile.orig
sed '/^FLAGS =/s/$/ -march=native/' Makefile.orig > Makefile
make
mkdir /usr/include/bfp
cp lib/libbfp.a /usr/lib
cp lib/posit_types.h /usr/include/bfp
cp lib/posit.h /usr/include/bfp