#cython: language_level=3

import gymnasium as gym
from libcpp.vector cimport vector as cpp_vector

# import C++ functions
cdef extern from "chvi.hpp":
    size_t cpp_chvi(env)

cdef public size_t action_space_size(env):
    return env.action_space.n

cdef public cpp_vector[size_t] observation_space_size(env):
    return env.observation_space.nvec

cdef public size_t execute_action(env, cpp_vector[size_t] state, size_t action):
    assert env.observation_space.contains(state), f"State {state} not part of the observation space"
    assert env.action_space.contains(action), f"Action {action} not part of the action space"
    print(f"Received state: {state} (type: {type(state)})")
    print(f"Received action: {action} (type: {type(action)})")
    #env.reset(state)
    #next_state, rewards, d, _ = env.step(action)
    return id(env)

def run(env):
    assert isinstance(env.observation_space, gym.spaces.MultiDiscrete), "Only gym.spaces.MultiDiscrete observation spaces are supported"
    assert isinstance(env.action_space, gym.spaces.Discrete), "Only gym.spaces.Discrete action spaces are supported"
    assert id(env) == cpp_chvi(env)
