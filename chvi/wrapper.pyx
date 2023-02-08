#cython: language_level=3

import gymnasium as gym
import numpy as np
from libcpp.vector cimport vector as cpp_vector


# import C++ functions
cdef extern from "chvi.hpp":
    size_t cpp_chvi(env)


cdef public cpp_vector[size_t] get_state(env):
    return env.state


cdef public size_t get_action_space_size(env):
    return env.action_space.n


cdef public cpp_vector[size_t] get_observation_space_size(env):
    return env.observation_space.nvec


cdef public cpp_vector[double] get_action_rewards(env, cpp_vector[size_t] state, size_t action):
    assert env.observation_space.contains(state), f"State {state} not part of the observation space"
    assert env.action_space.contains(action), f"Action {action} not part of the action space"
    env.reset()
    env.state = env.unwrapped.state = np.array(state) # probably not the most memory-optimized way
    _, rewards, _, _ = env.step(action)
    return np.atleast_1d(rewards)


def run(env):
    assert isinstance(env.observation_space, gym.spaces.MultiDiscrete), "Only gym.spaces.MultiDiscrete observation spaces are supported"
    assert isinstance(env.action_space, gym.spaces.Discrete), "Only gym.spaces.Discrete action spaces are supported"
    env.reset()
    cpp_chvi(env)
