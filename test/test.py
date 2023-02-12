#!/usr/bin/python3

import gymnasium as gym
import numpy as np
import random
import sys

from typing import Optional
from rich.table import Column
from rich.text import Text
from rich.progress import (
    BarColumn,
    Progress,
    TextColumn,
    ProgressColumn,
    TimeElapsedColumn,
    TimeRemainingColumn,
)

from CHVI import sweeping, partial_convex_hull_value_iteration
import chvi


class MofNCompleteColumn(ProgressColumn):
    """Renders completed count/total, e.g. '  10/1000'.
    Best for bounded tasks with int quantities.
    Space pads the completed count so that progress length does not change as task progresses
    past powers of 10.
    Args:
        separator (str, optional): Text to separate completed and total values. Defaults to "/".
    """

    def __init__(self, separator: str = "/", table_column: Optional[Column] = None):
        self.separator = separator
        super().__init__(table_column=table_column)

    def render(self, task: "Task") -> Text:
        """Show completed/total."""
        completed = int(task.completed)
        total = int(task.total) if task.total is not None else "?"
        total_width = len(str(total))
        return Text(
            f"{completed:{total_width}d}{self.separator}{total}",
            style="progress.download",
        )


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


if __name__ == "__main__":

    discount_factor = 1.0
    n_tests = 100

    # Define custom progress bar
    progress_bar = Progress(
        TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
        BarColumn(),
        MofNCompleteColumn(),
        TextColumn("•"),
        TimeElapsedColumn(),
        TextColumn("•"),
        TimeRemainingColumn(),
    )

    with progress_bar as p:
        for i in p.track(range(n_tests)):
            seed = random.randint(0, sys.maxsize)
            env = TestEnv([4, 7], 50, seed)
            V = [np.array([])] * env.n_states
            output1 = []
            #output1 = [[list(p) for p in hull] for hull in sweeping(0, env.n_states, env, V, discount_factor)]
            #print(partial_convex_hull_value_iteration(env, discount_factor, 1))
            env = TestEnv([4, 7], 50, seed)
            output2 = chvi.run(env, discount_factor=discount_factor, max_iterations=1, verbose=False)
            #print(output1)
            #print(output2)
            #if output1 != output2:
            #    print(f'Test failed for seed {seed}', file=sys.stderr)
            #    exit(1)
