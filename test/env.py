import gymnasium as gym
import numpy as np


class TestEnv(gym.Env):

    def __init__(self, observation_space_size, action_space_size, seed=0):
        self.seed = seed
        self.observation_space = gym.spaces.MultiDiscrete(observation_space_size, seed=seed)
        self.n_states = np.prod(observation_space_size)
        self.action_space = gym.spaces.Discrete(action_space_size, seed=seed)
        self.state = np.zeros(self.observation_space.shape)
        self.ex_pfx_product = np.ones(self.observation_space.shape, dtype=int)
        self.ex_pfx_product[1:] = np.cumprod(observation_space_size[:-1])

    def random(self, seed):
        a = 1103515245
        c = 12345
        m = 2**31
        seed = (a * seed + c) % m
        return seed

    def reset(self, state):
        self.state = state.copy()

    def step(self, action):
        rw = self.state + action
        self.state = self.state_scalar_to_vector(self.random(self.state_vector_to_scalar(self.state)) % self.n_states);
        return self.state, rw, self.is_terminal(self.state), False

    def is_terminal_scalar(self, scalar):
        return self.is_terminal(self.state_scalar_to_vector(scalar))

    def is_terminal(self, state):
        return np.equal(self.observation_space.nvec, state + 1).any()

    def state_vector_to_scalar(self, vector):
        return np.sum(np.multiply(vector, self.ex_pfx_product))

    def state_scalar_to_vector(self, scalar):
        return np.mod(np.floor_divide(scalar, self.ex_pfx_product), self.observation_space.nvec)

    def __str__(self):
        return f'{self.observation_space.nvec} {self.action_space.n}'
