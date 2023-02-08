#!/usr/bin/python3

import gymnasium as gym
import numpy as np
import chvi


class TestEnv(gym.Env):

    def __init__(self, observation_space_size, action_space_size):
        self.observation_space = gym.spaces.MultiDiscrete(observation_space_size)
        self.action_space = gym.spaces.Discrete(action_space_size)

    def reset(self):
        self.state = np.zeros(self.observation_space.shape)

    def step(self, action):
        print(f'Executing action {action} in state {self.state}')
        rw = self.state + action
        self.state = self.observation_space.sample()
        return self.state, rw, False, False


env = TestEnv([4, 4], 4)
chvi.run(env)
