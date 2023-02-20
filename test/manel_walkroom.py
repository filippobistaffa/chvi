import numpy as np
import itertools

from gymnasium.spaces import Box, Discrete, MultiDiscrete
from gymnasium.core import Env


def initializeGrid(size, dimensions, random_seed):
    np.random.seed(random_seed)
    grid = np.random.randint(size, size=size**dimensions)
    indices = np.random.choice(grid.shape[0], replace=False, size=int(grid.shape[0] * 0.8))
    if grid[0] == 0:
        grid[0] = size + 1  # initial point is never a solution
    grid[indices] = size + 1
    return grid


class WalkRoom(Env):

    def __init__(self, size=7, dimensions=4, seed=0):
        super(WalkRoom, self).__init__()
        self.size = size
        self.dimensions = dimensions
        self.room = self.make_boundaries(seed=seed)
        self.actions = np.zeros((self.dimensions*2, self.dimensions), dtype=int)
        # move forward
        self.actions[range(self.dimensions), range(self.dimensions)] = 1
        # move backward
        self.actions[range(self.dimensions, self.dimensions*2), range(self.dimensions)] = -1
        self.pos = np.zeros(self.dimensions, dtype=int)
        self.observation_space = MultiDiscrete(dimensions*(size,))
        self.n_states = np.prod(self.observation_space.nvec)
        self.action_space = Discrete(len(self.actions))
        self.reward_space = Box(-np.inf, 0, (dimensions,))
        self.reset()

    def make_boundaries(self, seed=0):
        d = self.dimensions-1
        room = initializeGrid(self.size, d, seed)
        room = room.reshape((self.size,)*d)
        print("The room:\n", room)
        return room
        
    def reset(self, forced_pos=None):
        self.pos = np.zeros(self.dimensions, dtype=int)
        if forced_pos is not None:
            self.pos = np.array(forced_pos)
        return self.pos.copy()

    def step(self, action):
        pos = self.pos + self.actions[action]
        pos = np.clip(pos, 0, self.size-1)
        reward = -np.abs(self.actions[action].copy())
        terminal = self.is_terminal(pos)
        # TODO: Añadido para que el CHVI no encuentre soluciones tontas
        if terminal:
            np.add(reward, np.ones(self.dimensions)*(self.size), out=reward, casting='unsafe')
        self.pos = pos
        return pos.copy(), reward, terminal, {}

    def render(self):
        txt = ''
        for y in range(self.size):
            for x in range(self.size):
                # TODO: La comparación era con self.pos a secas, pero lo cambio a self.pos[-1] para que haga algo
                if np.all(self.pos[-1] == np.array([x, y])):
                    char = 'X'
                else:
                    char = '#' if self.room[x,y] else '.'
                txt += char
            txt += '\n'
        return txt

    def is_terminal(self, pos):
        limit = self.room[tuple(pos[:-1].tolist())]
        return pos[-1] >= limit

    def is_terminal_scalar(self, scalar_pos):
        vect_pos = self.state_scalar_to_vector(scalar_pos)
        limit = self.room[tuple(vect_pos[:-1])]
        return vect_pos[-1] >= limit

    def terminal_list(self):
        terminals_no_value = np.argwhere(self.room < self.size)
        terminal_states = list()
        for t in terminals_no_value:
            t_value = self.room[tuple(t.tolist())]
            terminal_states.append(t.tolist() + [t_value])
        return terminal_states

    def state_scalar_to_vector(self, state):
        return [(state // self.size**(self.dimensions-j)) % self.size for j in range(1, self.dimensions + 1)]

    def state_vector_to_scalar(self, state):
        return np.sum(state[self.dimensions-j] * self.size**(j-1) for j in range(1, self.dimensions + 1))


if __name__ == "__main__":
    env = WalkRoom(4, 3, 1)
