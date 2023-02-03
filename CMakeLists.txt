cmake_minimum_required(VERSION 3.1)
project(chvi)
set(CMAKE_CXX_STANDARD 17)
include_directories(fmt/include)
add_executable(chvi main.cpp ch.cpp)
target_link_libraries(chvi PRIVATE qhullcpp)
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
target_compile_options(chvi PRIVATE ${PEDANTIC_COMPILE_FLAGS} ${OPTIMIZATION_COMPILE_FLAGS})
