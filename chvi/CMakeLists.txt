cmake_minimum_required(VERSION 3.1)
option(BUILD_CYTHON "Build Cython Extension" ON)

set(NAME chvi)
project(${NAME})
set(CMAKE_CXX_STANDARD 17)

# https://github.com/PointCloudLibrary/pcl/blob/master/cmake/Modules/FindQhull.cmake
include("FindQhull.cmake")
if (NOT QHULL_FOUND)
  message( FATAL_ERROR "Qhull library and development packages are required!" )
endif()

set(PEDANTIC_COMPILE_FLAGS
    -pedantic-errors
    -Wall
    -Wextra
    -pedantic
)

set(OPTIMIZATION_COMPILE_FLAGS
    -Ofast
    -march=native
    -funroll-loops
    -funsafe-loop-optimizations
    -falign-functions=16
    -falign-loops=16
    -fopenmp
)

set(SOURCES
    convex_hull.cpp
    pagmo.cpp
)

include_directories(fmt/include)
include_directories(${CMAKE_BINARY_DIR}/${NAME})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "Include '${dir}'")
endforeach()

if (BUILD_CYTHON)
    add_cython_target(wrapper CXX)
    add_library(wrapper MODULE ${wrapper} chvi.cpp ${SOURCES})
    python_extension_module(wrapper)
    install(TARGETS wrapper LIBRARY DESTINATION ${NAME})
    target_compile_options(wrapper PRIVATE ${PEDANTIC_COMPILE_FLAGS} ${OPTIMIZATION_COMPILE_FLAGS})
    target_compile_definitions(wrapper PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
    target_link_libraries(wrapper qhullcpp)
else()
    add_executable(${NAME} main.cpp ${SOURCES})
    target_compile_options(${NAME} PRIVATE ${PEDANTIC_COMPILE_FLAGS} ${OPTIMIZATION_COMPILE_FLAGS})
    target_link_libraries(${NAME} PRIVATE qhullcpp)
endif()
