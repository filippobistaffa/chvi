cmake_minimum_required(VERSION 3.1)
option(BUILD_CYTHON "Build Cython Extension" ON)

set(NAME chvi)
project(${NAME})
set(CMAKE_CXX_STANDARD 20)
set(MINIMUM_QHULL_VERSION 8.1) # fixes several bugs of versions 8.0.x

# https://github.com/PointCloudLibrary/pcl/blob/master/cmake/Modules/FindQhull.cmake
include("FindQhull.cmake")
if (NOT QHULL_FOUND)
    message( FATAL_ERROR "Qhull library and development packages are required!" )
endif()

if(${Qhull_VERSION} VERSION_LESS ${MINIMUM_QHULL_VERSION})
    message( FATAL_ERROR "Qhull version >= 8.1 is required!" )
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

include_directories(fmt/include)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "Include '${dir}'")
endforeach()

if (BUILD_CYTHON)
    include_directories(${CMAKE_BINARY_DIR}/${NAME})
    add_cython_target(wrapper CXX)
    add_library(wrapper MODULE ${wrapper} chvi.cpp)
    python_extension_module(wrapper)
    install(TARGETS wrapper LIBRARY DESTINATION ${NAME})
    target_compile_options(wrapper PRIVATE ${PEDANTIC_COMPILE_FLAGS} ${OPTIMIZATION_COMPILE_FLAGS})
    target_compile_definitions(wrapper PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
    target_compile_definitions(wrapper PRIVATE CYTHON)
    target_link_libraries(wrapper qhullcpp qhullstatic_r)
else()
    find_package(OpenMP REQUIRED)
    add_executable(${NAME} main.cpp chvi.cpp)
    target_compile_options(${NAME} PRIVATE ${PEDANTIC_COMPILE_FLAGS} ${OPTIMIZATION_COMPILE_FLAGS})
    target_link_libraries(${NAME} PRIVATE qhullcpp qhullstatic_r OpenMP::OpenMP_CXX)
endif()
