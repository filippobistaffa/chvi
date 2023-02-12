#!/usr/bin/python3

import gymnasium as gym
import numpy as np
import random
import sys

from typing import Optional
from rich.console import Console
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
    max_seed = sys.maxsize
    n_tests = 1000

    seeds = np.random.randint(max_seed, size=n_tests)

    # Define custom progress bar
    test_progress = Progress(
        TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
        BarColumn(),
        MofNCompleteColumn(),
        TextColumn("•"),
        TimeElapsedColumn(),
        TextColumn("•"),
        TimeRemainingColumn(),
    )

    def list_of_sets_of_tuples(x):
        return [{tuple(z) for z in y} for y in x]

    with test_progress as progress:
        task = progress.add_task("Testing...", total=len(seeds))
        for seed in seeds:
            env = TestEnv([4, 7], 50, int(seed))
            V = [np.array([])] * env.n_states
            output1 = list_of_sets_of_tuples(sweeping(0, env.n_states, env, V, discount_factor))
            #print(partial_convex_hull_value_iteration(env, discount_factor, 1))
            env = TestEnv([4, 7], 50, int(seed))
            output2 = list_of_sets_of_tuples(chvi.run(env, discount_factor=discount_factor, max_iterations=1, verbose=False))
            if output1 == output2:
                progress.console.print(f'Testing seed {seed:>0{len(str(max_seed))}} [[bold green]PASSED[/]]')
                progress.update(task, advance=1)
            else:
                progress.console.print(f'Testing seed {seed:>0{len(str(max_seed))}} [[bold red]FAILED[/]]')
                for (a, b) in zip(output1, output2):
                    if a != b:
                        print(f'{a} != {b}')
