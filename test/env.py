import gymnasium as gym
import numpy as np


class TestEnv(gym.Env):

    def __init__(self, dimensions, size, seed=0, goal_percentage=0.01):
        self.dimensions = dimensions
        self.size = size
        self.seed = seed
        observation_space_size = np.full(dimensions, size)
        action_space_size = 2 * dimensions
        self.observation_space = gym.spaces.MultiDiscrete(observation_space_size, seed=seed)
        self.n_states = np.prod(observation_space_size)
        self.action_space = gym.spaces.Discrete(action_space_size, seed=seed)
        self.state = np.zeros(self.observation_space.shape)
        self.ex_pfx_product = np.ones(self.observation_space.shape, dtype=int)
        self.ex_pfx_product[1:] = np.cumprod(observation_space_size[:-1])
        # generate goal set
        self.goals = set()
        n_goals = max(1, int(goal_percentage * size**dimensions))
        for goal in range(n_goals):
            p = np.empty(dimensions)
            while True:
                for dimension in range(dimensions):
                    seed = self.random(seed)
                    p[dimension] = seed % size
                if np.any(p) and not tuple(p) in self.goals:
                    break
            self.goals.add(tuple(p))
        #print(len(self.goals))
        #print(self.goals)

    def random(self, seed):
        a = 1103515245
        c = 12345
        m = 2**31
        return (a * seed + c) % m

    def reset(self, state):
        self.state = state.copy()

    def step(self, action):
        dimension = action // 2
        step = 2 * (action % 2) - 1
        self.state[dimension] += step;
        self.state = np.clip(self.state, 0, self.size - 1)
        rw = np.zeros(self.dimensions)
        rw[dimension] = -1;
        return self.state, rw, self.is_terminal(self.state), False

    def is_terminal_scalar(self, scalar):
        return self.is_terminal(self.state_scalar_to_vector(scalar))

    def is_terminal(self, state):
        return tuple(state) in self.goals

    def state_vector_to_scalar(self, vector):
        return np.sum(np.multiply(vector, self.ex_pfx_product))

    def state_scalar_to_vector(self, scalar):
        return np.mod(np.floor_divide(scalar, self.ex_pfx_product), self.observation_space.nvec)

    def __str__(self):
        return f'{self.observation_space.nvec} {self.action_space.n}'


if __name__ == "__main__":
    env = TestEnv(3, 3, 2671936)
