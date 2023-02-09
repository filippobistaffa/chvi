#ifndef CHVI_HPP_
#define CHVI_HPP_

#include "Python.h"

void run_chvi(PyObject *env, const double discount_factor, const std::size_t max_iterations, const double epsilon);

#endif
