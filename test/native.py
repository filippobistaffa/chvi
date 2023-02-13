#!/usr/bin/python3

from subprocess import PIPE
import subprocess
import numpy as np
import random
import time
import sys
import os

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

from env import TestEnv
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


if __name__ == "__main__":

    def list_of_sets_of_tuples(x):
        return [{tuple(z) for z in y} for y in x]

    # environment parameters
    max_dimensions = 8
    max_space_size = 20
    max_actions = 30

    # algorithm parameters
    discount_factor = 1.0
    max_iterations = 1000
    epsilon = 0.01

    # tests parameters
    width = 10
    n_tests = 1000
    max_seed = 2**32 - 1
    seeds = np.random.randint(max_seed, size=n_tests)
    steps = 100

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
            np.random.seed(seed)
            dimensions = np.random.randint(2, max_dimensions + 1, size=1).item()
            space_size = np.random.randint(2, max_space_size + 1, size=dimensions)
            actions = np.random.randint(2, max_actions + 1, size=1).item()
            start_time = time.time()
            env = TestEnv(space_size, actions, int(seed))
            python = []
            for step in range(steps):
                n, r, _, _ = env.step(step)
                python.append((list(n), list(r)))
            t1 = f'{time.time()-start_time:.{width}f}'
            command_line = [exe_abs_path, str(len(space_size))]
            command_line.extend(str(x) for x in space_size)
            command_line.extend(str(x) for x in [actions, seed, steps])
            start_time = time.time()
            output = subprocess.run(command_line, check=True, stdout=PIPE, stderr=PIPE).stdout.decode().rstrip()
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
