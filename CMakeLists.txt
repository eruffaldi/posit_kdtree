add_definitions(--std=c++11)

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
add_executable(nanoflann_kdd src/nanoflann_kdd.cpp)
target_link_libraries(nanoflann_kdd posittab)

add_library(nanotest_kdd STATIC src/kdtree_interface.cpp)
target_link_libraries(nanotest_kdd posittab softfloat16 softfloat)

add_executable(nanotest_kdd_test src/kdtree_interface_test.cpp)
target_link_libraries(nanotest_kdd_test nanotest_kdd)
add_subdirectory(python)
