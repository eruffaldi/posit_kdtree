RUN git clone https://USER:PASSWORD@bitbucket.org/eruffaldi/posit_kdtree.git 
RUN cd posit_kdtree && git clone --recurse-submodules https://github.com/eruffaldi/cppPosit.git && rmdir cppposit && mv cppPosit cppposit

RUN cd posit_kdtree/cppposit/extern  && git clone https://github.com/ucb-bar/berkeley-softfloat-3.git && mv berkeley-softfloat-3 softfloat
RUN cd posit_kdtree/cppposit/extern/softfloat/build/Linux-x86_64-GCC/ && make
RUN cd posit_kdtree/cppposit/extern && mkdir lib
RUN cp posit_kdtree/cppposit/extern/softfloat/build/Linux-x86_64-GCC/softfloat.a posit_kdtree/cppposit/extern/libsoftfloat.a
RUN cp posit_kdtree/cppposit/extern/softfloat/build/Linux-x86_64-GCC/softfloat.a posit_kdtree/libsoftfloat.a

RUN touch posit_kdtree/cppposit/extern/softfloat/CMakeLists.txt

RUN alias python=python3

#RUN cd posit_kdtree && sed -i '1 i\ cmake_minimum_required(VERSION 3.10)' CMakeLists.txt
RUN cd posit_kdtree/include && sed -i '1 i\#include <memory>' kdtree_interface.hpp && cat kdtree_interface.hpp
RUN cd posit_kdtree/include && sed -i '1 i\#include <chrono>' kdtree_interface.hpp && cat kdtree_interface.hpp
RUN cd posit_kdtree && cmake -DPython_ADDITIONAL_VERSIONS=3.7 -DPYTHON_LIBRARIES="/usr/lib/libpython37.dylib" -DPYTHON_INCLUDE_DIRS="/usr/include/python/3.7" -DPYTHON_EXECUTABLE="/usr/bin/python3" -DPYTHON_VERSION_SUFFIX=3 -DCMAKE_POSITION_INDEPENDENT_CODE=ON .

RUN cd posit_kdtree && make
RUN cd posit_kdtree/python && python3 -c 'import nanoflanns'
RUN cd posit_kdtree/python && python3 -c 'import nanoflanns2'