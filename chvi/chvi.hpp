#ifndef CHVI_HPP_
#define CHVI_HPP_

#include "types.hpp" // std::vector, point type

#ifdef CYTHON
#include "Python.h"
std::vector<std::vector<point>> run_chvi(PyObject *env, const double discount_factor, const std::size_t max_iterations, const double epsilon, const bool verbose);
#else
//
#endif

#endif
