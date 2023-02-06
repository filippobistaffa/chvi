cmake_minimum_required(VERSION 3.1)
set(NAME chvi)
project(${NAME})
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(pagmo2)
include_directories(pagmo2/include)
include_directories(fmt/include)
add_executable(${NAME} main.cpp convex_hull.cpp)
target_link_libraries(${NAME} PRIVATE qhullcpp pagmo)
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
target_compile_options(${NAME} PRIVATE ${PEDANTIC_COMPILE_FLAGS} ${OPTIMIZATION_COMPILE_FLAGS})
