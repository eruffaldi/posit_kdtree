message(STATUS Python_ADDITIONAL_VERSIONS ${Python_ADDITIONAL_VERSIONS})
FIND_PACKAGE(PythonInterp 3 REQUIRED)
FIND_PACKAGE(PythonLibs 3 REQUIRED)

#${PYTHON_VERSION_SUFFIX} 
if (PYTHONINTERP_FOUND)
    message(STATUS Python found is ${PYTHON_VERSION_MAJOR})
  if (UNIX AND NOT APPLE)
    if (PYTHON_VERSION_MAJOR EQUAL 3)
        FIND_PACKAGE(Boost COMPONENTS python-py${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR} REQUIRED)
        #FIND_PACKAGE(Boost COMPONENTS python${PYTHON_VERSION_SUFFIX} REQUIRED)
        #FIND_PACKAGE(Boost COMPONENTS python REQUIRED)
    else()
        FIND_PACKAGE(Boost COMPONENTS python)
    endif()
  else()	
    FIND_PACKAGE(Boost COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR} REQUIRED)
  endif()
else()
    message("Python not found")
endif()

message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES}) # Deprecated but so convenient!

PYTHON_ADD_MODULE(extest extest.cpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/extest.py ${CMAKE_CURRENT_BINARY_DIR}/extest.py COPYONLY)

#PYTHON_ADD_MODULE(nanoflanns nanoflanns.cpp)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanotest.py ${CMAKE_CURRENT_BINARY_DIR}/nanotest.py COPYONLY)

PYTHON_ADD_MODULE(pynanoflann_any pynanoflann_any.cpp ../extern/backward.cpp)
target_link_libraries(pynanoflann_any nanoflann_any)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoflann_any.py ${CMAKE_CURRENT_BINARY_DIR}/nanoflann_any.py COPYONLY)
