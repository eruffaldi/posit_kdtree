#here starts ann-benchmarks
#FROM ann-benchmarks
FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y python3-numpy python3-scipy python3-pip build-essential git

COPY requirements.txt .
RUN pip3 install -rrequirements.txt

# here starts our
RUN apt-get update && apt-get install -y cmake
RUN apt-get install -y clang-3.9 lldb-3.9
RUN apt-get install -y python3.7
RUN apt-get install -y python3-dev
RUN apt-get install -y libboost-all-dev
RUN apt-get -y install python3-pip
RUN apt install libeigen3-dev
RUN pip3 install -U lxml

COPY deploy_softfloat.h . 
RUN ./deploy_softfloat.h

#provided with posit_kdtree
#COPY deploy_cppposit.h . 
#RUN ./deploy_cppposit.h

#assume having posit_kdtree here
ARG gituser
ARG gitpass
RUN git clone https://${gituser}:${gitpass}@bitbucket.org/eruffaldi/posit_kdtree.git 
RUN cd posit_kdtree && git clone --depth 1 --recurse-submodules https://github.com/eruffaldi/cppPosit.git
RUN cd posit_kdtree && cmake -DPython_ADDITIONAL_VERSIONS=3.7 -DPYTHON_LIBRARIES="/usr/lib/libpython37.dylib" -DPYTHON_INCLUDE_DIRS="/usr/include/python/3.7" -DPYTHON_EXECUTABLE="/usr/bin/python3" -DPYTHON_VERSION_SUFFIX=3 -DCMAKE_POSITION_INDEPENDENT_CODE=ON .
RUN cd posit_kdtree && make
RUN cd posit_kdtree/python && python3 -c 'import nanoflanns'
RUN cd posit_kdtree/python && python3 -c 'import nanoflanns2'