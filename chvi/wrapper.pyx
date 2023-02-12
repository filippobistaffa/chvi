#cython: language_level=3

import gymnasium as gym
import numpy as np
from libcpp.vector cimport vector as cpp_vector
from libcpp.pair cimport pair as cpp_pair
from libcpp cimport bool


# import C++ functions
cdef extern from "chvi.hpp":
    cpp_vector[cpp_vector[cpp_vector[double]]] run_chvi(env, double discount_factor, size_t max_iterations, double epsilon, bool verbose)


cdef public cpp_vector[size_t] get_state(env):
    return env.state


cdef public size_t get_action_space_size(env):
    return env.action_space.n


cdef public cpp_vector[size_t] get_observation_space_size(env):
    return env.observation_space.nvec


cdef public cpp_pair[cpp_vector[size_t],cpp_vector[double]] execute_action(env, cpp_vector[size_t] state, size_t action):
    assert env.observation_space.contains(state), f"State {state} not part of the observation space"
    assert env.action_space.contains(action), f"Action {action} not part of the action space"
    #env.reset() # not sure if needed
    env.state = env.unwrapped.state = np.array(state) # probably not the most memory-optimized way
    next_state, rewards, _, _ = env.step(action)
    return cpp_pair[cpp_vector[size_t],cpp_vector[double]] (next_state, np.atleast_1d(rewards))


def run(env, discount_factor=1.0, max_iterations=100, epsilon=0.01, verbose=True):
    assert isinstance(env.observation_space, gym.spaces.MultiDiscrete), "Only gym.spaces.MultiDiscrete observation spaces are supported"
    assert isinstance(env.action_space, gym.spaces.Discrete), "Only gym.spaces.Discrete action spaces are supported"
    assert 'state' in dir(env), 'Environment needs to store current state in an attribute called "state"'
    assert isinstance(env.state, np.ndarray), "State attribute must be a np.ndarray"
    return run_chvi(env, discount_factor, max_iterations, epsilon, verbose)
