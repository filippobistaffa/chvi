import numpy as np
import itertools

from gymnasium.spaces import Box, Discrete, MultiDiscrete
from gymnasium.core import Env
from manel_convexhull import get_hull


def non_dominated(solutions, return_indexes=False):
    is_efficient = np.ones(solutions.shape[0], dtype=bool)
    for i, c in enumerate(solutions):
        if is_efficient[i]:
            # Remove dominated points, will also remove itself
            is_efficient[is_efficient] = np.any(solutions[is_efficient] > c, axis=1)
            # keep this solution as non-dominated
            is_efficient[i] = 1
    if return_indexes:
        return solutions[is_efficient], is_efficient
    else:
        return solutions[is_efficient]


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
        # pareto_front_i = [c + (self.room[c],) for c in (np.unravel_index(i, self.room.shape) for i in nd_i)]
        # self.pareto_front_b = np.array(pareto_front_i)*-1
        # possible actions, do not move in diagonal
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

    @property
    def pareto_front(self):
        d = self.dimensions-1
        boundary = [coord + (self.room[coord],) for coord in itertools.product(*(np.arange(self.size),)*d)]
        pf = non_dominated(np.array(boundary)*-1)
        return pf

    def make_boundaries(self, seed=0):
        d = self.dimensions-1
        room = initializeGrid(self.size, d, seed)
        room = room.reshape((self.size,)*d)
        #print("The room: ", room)
        return room
        
    def reset(self, forced_pos=None):
        self.pos = np.zeros(self.dimensions, dtype=int)
        if forced_pos is not None:
            self.pos = np.array(forced_pos)
        return self.pos.copy()

    def step(self, action):
        pos = self.pos + self.actions[action]
        # stay in room bounds
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


def random_walkroom(size, jumps=2, seed=0):
    rng = np.random.default_rng(seed)
    # make sure that, on average you have one jump per step
    p = np.ones(jumps)* 1/np.arange(jumps).sum()
    p[0] = 1-(jumps-1)*p[1]
    y = rng.choice(np.arange(jumps), size=size, p=p).cumsum()[::-1]
    return WalkRoom(size, y)


def voxel_representation(env, s, d):
    import matplotlib.pyplot as plt
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    voxels = np.zeros((s,)*d, dtype=bool)
    for coord in itertools.product(*(np.arange(s),)*(d-1)):
        if 0 < env.room[coord] < env.size:
            voxels[coord+(env.room[coord],)] = True
    ax.voxels(voxels)
    plt.show()
