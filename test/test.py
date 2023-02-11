#!/usr/bin/python3

import gymnasium as gym
import numpy as np
import random
import sys
import chvi


from CHVI import sweeping, partial_convex_hull_value_iteration


class TestEnv(gym.Env):

    def __init__(self, observation_space_size, action_space_size, seed=0):
        self.observation_space = gym.spaces.MultiDiscrete(observation_space_size, seed=seed)
        self.n_states = np.prod(observation_space_size)
        self.action_space = gym.spaces.Discrete(action_space_size, seed=seed)
        self.state = np.zeros(self.observation_space.shape)
        self.ex_pfx_product = np.ones(self.observation_space.shape, dtype=int)
        self.ex_pfx_product[1:] = np.cumprod(observation_space_size[:-1])

    def reset(self, state):
        self.state = state

    def step(self, action):
        rw = self.state + action
        self.state = self.observation_space.sample()
        return self.state, rw, False, False

    def is_terminal_scalar(self, scalar):
        return False

    def state_vector_to_scalar(self, vector):
        return np.sum(np.multiply(vector, self.ex_pfx_product))

    def state_scalar_to_vector(self, scalar):
        return np.mod(np.floor_divide(scalar, self.ex_pfx_product), self.observation_space.nvec)


discount_factor = 1.0
seed = random.randint(0, sys.maxsize)

env = TestEnv([4, 7], 50, seed)
V = [np.array([])] * env.n_states
output1 = [[list(p) for p in hull] for hull in sweeping(0, env.n_states, env, V, discount_factor)]
#print(partial_convex_hull_value_iteration(env, discount_factor, 1))

env = TestEnv([4, 7], 50, seed)
output2 = chvi.run(env, discount_factor, 1)

print(output1)
print(output2)
print(output1 == output2)
