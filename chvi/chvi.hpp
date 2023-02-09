#ifndef CHVI_HPP_
#define CHVI_HPP_

#include "Python.h"

void run_chvi(PyObject *env, double discount_factor, std::size_t max_iterations, double epsilon);

#endif
