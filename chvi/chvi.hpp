#ifndef CHVI_HPP_
#define CHVI_HPP_

#include "Python.h"
#include "types.hpp"    // point type

std::vector<std::vector<point>> run_chvi(PyObject *env, const double discount_factor, const std::size_t max_iterations, const double epsilon, const bool verbose);

#endif
