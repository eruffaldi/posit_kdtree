add_definitions(--std=c++14)

file(GLOB RESULT ${CMAKE_CURRENT_LIST_DIR}/cppPosit)
list(LENGTH RESULT RES_LEN)
if(RES_LEN EQUAL 0)
#not loaded
find_path(CPPPOSIT_INCLUDE_DIR posit.h PATH_SUFFIXES cppPosit)
include_directories(${CPPPOSIT_INCLUDE_DIR})
else()
add_subdirectory(cppPosit)
include_directories(cppPosit/include)
include_directories(cppPosit/extern)
# DIR is empty, do something
endif()
set(Python_ADDITIONAL_VERSIONS "" CACHE STRING "preferred python version (system for default,otherwise 3.7)")
include_directories(include)
include_directories(extern)
add_executable(nanoflann_kdd tests/nanoflann_kdd.cpp)
target_link_libraries(nanoflann_kdd posittab)

find_library(SOFTPOSIT_LIBRARY softposit)
find_library(SOFTPOSIT_INCLUDE softposit_cpp.h)

if(SOFTPOSIT_INCLUDE AND SOFTPOSIT_LIBRARY)
message("WITH softposit")
include_directories(${SOFTPOSIT_INCLUDE})
endif()

find_library(SOFTPOSIT_LIBRARY softposit)
find_library(STILLWATER_INCLUDE posit/posit_decoded.h)

if(STILLWATER_INCLUDE)
message("WITH stillwater")
include_directories(${STILLWATER_INCLUDE})
endif()

find_library(HALF_INCLUDE half.h)

if(HALF_INCLUDE)
message("WITH half")
include_directories(${HALF_INCLUDE})
endif()

add_library(nanoflann_any STATIC src/kdtree_interface.cpp src/kdtree_interface_fixedpoint.cpp src/kdtree_interface_cppPosit.cpp src/kdtree_interface_native.cpp src/kdtree_interface_softfloat.cpp src/kdtree_interface_softPosit.cpp src/kdtree_interface_stillwater.cpp src/kdtree_interface_half.cpp)
target_link_libraries(nanoflann_any posittab softfloat16 softfloat ${SOFTPOSIT_LIBRARY})

add_executable(nanoflann_any_test tests/kdtree_interface_test.cpp)
target_link_libraries(nanoflann_any_test nanoflann_any)
add_subdirectory(python)
