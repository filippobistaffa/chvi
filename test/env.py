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
        # initialize PRNG
        upper_seed = seed >> 32
        lower_seed = seed & 0xFFFFFFFF
        rng = self.pgc_initialize(upper_seed, lower_seed)
        for goal in range(n_goals):
            p = np.empty(dimensions)
            while True:
                for dimension in range(dimensions):
                    random = self.pgc_get_random(rng)
                    p[dimension] = random % size
                if np.any(p) and not tuple(p) in self.goals:
                    break
            self.goals.add(tuple(p))
        #print(len(self.goals))
        #print(self.goals)

    # https://www.pcg-random.org/

    def pgc_get_random(self, rng):
        oldstate = rng['state']
        # Advance internal state
        rng['state'] = (oldstate * 6364136223846793005 + (rng['inc']|1)) & 0xFFFFFFFFFFFFFFFF
        # Calculate output function (XSH RR), uses old state for max ILP
        xorshifted = (((oldstate >> 18) ^ oldstate) >> 27) & 0xFFFFFFFF
        rot = (oldstate >> 59) & 0xFFFFFFFF
        return ((xorshifted >> rot) | (xorshifted << ((-rot) & 31))) & 0xFFFFFFFF

    def pgc_initialize(self, initstate, initseq):
        rng = {
            'state': 0,
            'inc': (initseq << 1) | 1
        }
        self.pgc_get_random(rng)
        rng['state'] += initstate
        self.pgc_get_random(rng)
        return rng

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
