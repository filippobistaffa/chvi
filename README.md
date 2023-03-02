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

Compile Qhull with PIC and Optimizations
----------
1. Navigate to the `chvi/qhull` subdirectory
2. Adjust the desired optimization options in the [`CMakeList.txt`](https://github.com/filippobistaffa/qhull/blob/35f6b7ee396f6bd3e3a4832bc7342dc7909d5a5d/CMakeLists.txt#L447) file. Levels `-O3` and above can potentially affect the output of Qhull for some degenerate instances.
3. Build and install Qhull with `cmake .`, `make` and `sudo make install`

Build and Install the Cython Module
----------
1. Download required packages with `pip install -r requirements.txt`
2. Build the Cython module and package it in a Python Wheel with `./build.sh`
3. Install the Python Wheel with `./install`

Build the Native C++ Version
----------
1. Navigate to the `chvi` subdirectory
2. Compile the C++ native version with `./build.sh`
