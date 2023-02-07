#cython: language_level=3

import gymnasium as gym
from libcpp.vector cimport vector as cpp_vector

# import C++ functions
cdef extern from "chvi.hpp":
    long cpp_chvi(env)

cdef public long execute_action(env, cpp_vector[size_t] state, cpp_vector[size_t] action):
    print(f"Received environment ID: {id(env)}")
    print(f"Received state: {state} (type: {type(state)})")
    #env.reset(state)
    #next_state, rewards, d, _ = env.step(action)
    return id(env)

def run(env):
    print(f"We're in Python")
    print(f"Convex Hull Value Iteration")
    print(f"Environment ID: {id(env)}")
    assert id(env) == cpp_chvi(env)
