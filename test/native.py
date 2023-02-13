#!/usr/bin/python3

import gymnasium as gym
import numpy as np
import random
import time
import sys
import os
import subprocess
from subprocess import PIPE

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
        self.seed = seed
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
        exponents = np.arange(1, 1 + len(self.observation_space))
        self.state = np.mod(np.multiply(self.seed, exponents) + np.multiply(self.state, self.state), self.observation_space.nvec)
        return self.state, rw, self.is_terminal(self.state), False

    def is_terminal_scalar(self, scalar):
        return self.is_terminal(self.state_scalar_to_vector(scalar))

    def is_terminal(self, state):
        return np.equal(self.observation_space.nvec, state + 1).any()

    def state_vector_to_scalar(self, vector):
        return np.sum(np.multiply(vector, self.ex_pfx_product))

    def state_scalar_to_vector(self, scalar):
        return np.mod(np.floor_divide(scalar, self.ex_pfx_product), self.observation_space.nvec)


if __name__ == "__main__":

    def list_of_sets_of_tuples(x):
        return [{tuple(z) for z in y} for y in x]

    # environment parameters
    space_size = [9, 9]
    actions = 4

    # algorithm parameters
    discount_factor = 1.0
    max_iterations = 1000
    epsilon = 0.01

    # tests parameters
    width = 10
    n_tests = 10
    max_seed = 1e10
    seeds = np.random.randint(max_seed, size=n_tests)
    steps = 10

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

    exe_subdir = 'chvi'
    exe_name = 'chvi'
    exe_abs_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), exe_subdir, exe_name)

    with test_progress as progress:
        task = progress.add_task("Testing...", total=len(seeds))
        for seed in seeds:
            start_time = time.time()
            env = TestEnv(space_size, actions, int(seed))
            python = []
            for step in range(steps):
                n, r, _, _ = env.step(step)
                python.append((list(n), list(r)))
            t1 = f'{time.time()-start_time:.{width}f}'
            start_time = time.time()
            output = subprocess.run([
                exe_abs_path,
                str(space_size[0]),
                str(space_size[1]),
                str(actions),
                str(seed),
                str(steps)
            ], check=True, stdout=PIPE, stderr=PIPE).stdout.decode().rstrip()
            exec(f'native = {output}')
            t2 = f'{time.time()-start_time:.{width}f}'
            if python == native:
                progress.console.print(f'Testing seed {seed:>0{len(str(max_seed))}} (runtimes = {t1[:width]} {t2[:width]}) [[bold green]PASSED[/]]')
                progress.update(task, advance=1)
            else:
                progress.console.print(f'Testing seed {seed:>0{len(str(max_seed))}} (runtimes = {t1[:width]} {t2[:width]}) [[bold red]FAILED[/]]')
                for (a, b) in zip(l1, l2):
                    if a != b:
                        print(f'{a} != {b}')
