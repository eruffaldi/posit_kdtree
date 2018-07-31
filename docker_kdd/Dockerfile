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

COPY deploy_cppposit.h . 
RUN ./deploy_cppposit.h