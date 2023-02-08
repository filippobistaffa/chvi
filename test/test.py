#!/usr/bin/python3

import gymnasium as gym
import numpy as np
import chvi


class TestEnv(gym.Env):

    def __init__(self, observation_space_size, action_space_size):
        self.observation_space = gym.spaces.MultiDiscrete(observation_space_size)
        self.action_space = gym.spaces.Discrete(action_space_size)
        self.state = np.zeros(self.observation_space.shape)

    def reset(self):
        self.state = np.zeros(self.observation_space.shape)

    def step(self, action):
        rw = self.state + action
        self.state = self.observation_space.sample()
        return self.state, rw, False, False


env = TestEnv([4, 4], 50)
chvi.run(env)
