import sys

parameters = {
    # environment parameters
    "max_dimensions": 3,
    "max_size": 3,
    "goals": 0.01,
    # algorithm parameters
    "discount_factor": 1.0,
    "max_iterations": 100,
    "epsilon": 0.0,
    # tests parameters
    "seed": 12345,
    "n_tests": 100,
    "max_seed": sys.maxsize
}
