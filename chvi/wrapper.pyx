#cython: language_level=3

import gymnasium as gym
from libcpp.vector cimport vector as cpp_vector

# import C++ functions
cdef extern from "chvi.hpp":
    void test_chvi()

cpdef public void execute(env, cpp_vector[size_t] state, cpp_vector[size_t] action):
    env.reset(state)
    next_state, rewards, d, _ = env.step(action)

def run(env):
    print(f"We're in Python")
    print(f"Convex Hull Value Iteration")
    print(f"Environment: {env}")
    test_chvi()
