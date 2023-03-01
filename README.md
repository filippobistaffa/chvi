Convex Hull Value Iteration
===================

Unofficial implementation of the algorithm proposed in the article "[Learning All Optimal Policies with Multiple Criteria](http://icml2008.cs.helsinki.fi/papers/257.pdf)" by Barrett and Narayanan (Proceedings of the International Conference on Machine Learning, 2008).

Download
----------
Clone this repository with `--recursive` to recursively clone all submodules.

Required Packages
----------
- [CMake](https://cmake.org/)
- A version of [g++](https://gcc.gnu.org/) supporting C++20
- [OpenMP](https://www.openmp.org/) for C++ parallelism
- [pip](https://pypi.org/project/pip/)

Compile Qhull with PIC and optimizations
----------
1. Navigate to the `chvi/qhull` subdirectory
2. Build and install Qhull with `cmake .`, `make` and `sudo make install`

Build and Install the Cython Module
----------
1. Download required packages with `pip install -r requirements.txt`
2. Build the Cython module and package it in a Python Wheel with `./build.sh`
3. Install the Python Wheel with `./install`

Build the Native C++ Version
----------
1. Navigate to the `chvi` subdirectory
2. Compile the C++ native version with `./build.sh`
