#!/usr/bin/python3

import gymnasium as gym
import chvi

class TestEnv(gym.Env):
    def __init__(self, observation_space_size, action_space_size):
        self.observation_space = gym.spaces.MultiDiscrete(observation_space_size)
        self.action_space = gym.spaces.Discrete(action_space_size)

env = TestEnv([4, 4], 4)
chvi.run(env)
