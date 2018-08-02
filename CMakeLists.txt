add_definitions(--std=c++11)

file(GLOB RESULT DIR)
list(LENGTH RESULT RES_LEN)
if(RES_LEN EQUAL 0)
#not loaded
find_path(CPPPOSIT_INCLUDE_DIR posit.h PATH_SUFFIXES cppPosit)
include_directories(${CPPPOSIT_INCLUDE_DIR})
else()
add_subdirectory(cppposit)
include_directories(cppposit/include)
include_directories(cppPosit/extern)
# DIR is empty, do something
endif()

include_directories(include)
include_directories(extern)
add_executable(nanoflann_kdd src/nanoflann_kdd.cpp)
target_link_libraries(nanoflann_kdd posittab)

add_library(nanotest_kdd STATIC src/kdtree_interface.cpp)
target_link_libraries(nanotest_kdd posittab softfloat16)

add_executable(nanotest_kdd_test src/kdtree_interface_test.cpp)
target_link_libraries(nanotest_kdd_test nanotest_kdd)
add_subdirectory(python)
