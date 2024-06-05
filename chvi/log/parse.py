import argparse as ap
import numpy as np
import subprocess
from subprocess import PIPE
import sys
import os
import re

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

from decimal import Decimal

def fexp(number):
    (sign, digits, exponent) = Decimal(number).as_tuple()
    return len(digits) + exponent - 1

def fman(number):
    return Decimal(number).scaleb(-fexp(number)).normalize()

parser = ap.ArgumentParser()
parser.add_argument('--dimensions', type=int, nargs=2, default=(2, 9))
parser.add_argument('--sizes', type=int, nargs=2, default=(2, 9))
parser.add_argument('--seeds', type=int, nargs=2, default=(1, 20))
parser.add_argument('--limit', type=int, default=10*60*60)
parser.add_argument('--latex', action='store_true')
parser.add_argument('--colors', action='store_true')
parser.add_argument('--heatmap', action='store_true')
parser.add_argument('--palette', type=str, default='Blues', choices=['Blues', 'Greys'])
args = parser.parse_args()

def n_to_last_line(filename, n=1):
    ps = subprocess.run(['tail' if n > 0 else 'head', '-n', str(abs(n)), filename], check=True, stdout=PIPE, stderr=PIPE)
    out = subprocess.run(['head' if n > 0 else 'tail', '-n', '1'], input=ps.stdout, stdout=PIPE, stderr=PIPE).stdout.decode().rstrip()
    return out

n_dimensions = args.dimensions[1] - args.dimensions[0] + 1
n_sizes = args.sizes[1] - args.sizes[0] + 1
n_seeds = args.seeds[1] - args.seeds[0] + 1
steps = [
    np.full((args.dimensions[1] + 1, args.sizes[1] + 1, args.seeds[1] + 1), np.inf),
    np.full((args.dimensions[1] + 1, args.sizes[1] + 1, args.seeds[1] + 1), np.inf),
    np.full((args.dimensions[1] + 1, args.sizes[1] + 1, args.seeds[1] + 1), np.inf)
]

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

with test_progress as progress:
    task = progress.add_task("Testing...", total=n_dimensions * n_sizes * n_seeds)
    for d in range(args.dimensions[0], args.dimensions[1] + 1):
        for n in range(args.sizes[0], args.sizes[1] + 1):
            for s in range(args.seeds[0], args.seeds[1] + 1):
                filename = f'{d}-{n}-{s}.out'
                if os.path.isfile(filename):
                    runtimes = n_to_last_line(filename, -3).split(',')
                    steps[0][d, n, s] = float(runtimes[3])
                    steps[1][d, n, s] = float(runtimes[4])
                    steps[2][d, n, s] = float(runtimes[5])
                progress.update(task, advance=1)

#total = step1 + step2 + step3
total = sum(steps)
steps.append(total)

for i in range(4):
    steps[i] = np.clip(steps[i], 0, args.limit)
    steps[i] = steps[i][args.dimensions[0]:, args.sizes[0]:, args.seeds[0]:] # objectives on rows, sizes on columns

means = [np.mean(step, axis=2) for step in steps]
stds = [np.std(step, axis=2) for step in steps]
total_mean = means.pop()
std_mean = stds.pop()

#for mean, std in zip(means, stds):
#    print(mean)
#    print(3 * std)

if args.heatmap:
    import seaborn as sns
    import matplotlib.pylab as plt
    from matplotlib.colors import LogNorm
    sns.set(font_scale=2.0)
    ax = sns.heatmap(total_mean.T, cmap=args.palette, norm=LogNorm()) # sizes on rows, objectives on columns, as in the paper
    #ax = sns.heatmap(total_mean.T, cmap=args.palette) # sizes on rows, objectives on columns, as in the paper
    ax.set_xlabel('Objectives')
    ax.set_xticklabels(list(range(args.dimensions[0], args.dimensions[1] + 1)))
    ax.set_ylabel('Size')
    ax.set_yticklabels(list(range(args.sizes[0], args.sizes[1] + 1)))
    plt.tight_layout()
    plt.show()
    print((total_mean == args.limit).sum())
elif args.latex:
    from itertools import product
    for (s, d) in product(range(args.sizes[0], args.sizes[1] + 1), range(args.dimensions[0], args.dimensions[1] + 1)):
        print(f'\\multicolumn{{1}}{{c|}}{{\\textbf{{({s}, {d})}}}} & ', end='')
        i = d - args.dimensions[0]
        j = s - args.sizes[0]
        mean_man_exp = [(fman(mean[i, j]), fexp(mean[i, j])) for mean in means]
        std_man_exp  = [(fman(3 * std[i, j]), fexp(3 * std[i, j])) for std in stds]
        cell_colors = [''] * 3
        cell_separators = ['&'] * 2 + [r'\\ \hhline{~|*{3}{-}}']
        if means[0][i, j] == args.limit:
            cell_values = ['$> 10$h'] + ['--'] * 2
            if args.colors:
                cell_colors = [r'\cellcolor[HTML]{\cellred}'] * 3
        else:
            cell_values = [
                f'${mean_man:.02f}' + r' \cdot 10^' + f'{{{mean_exp}}}' + r' \pm ' +
                f'{std_man:.02f}' + r' \cdot 10^' + f'{{{std_exp}}}$'
                for ((mean_man, mean_exp), (std_man, std_exp)) in zip(mean_man_exp, std_man_exp)
            ]
            for i, (_, exp) in enumerate(mean_man_exp):
                if exp == 0:
                    cell_colors[i] = r'\cellcolor[HTML]{\cellgreen}'
                elif exp == 1:
                    cell_colors[i] = r'\cellcolor[HTML]{\cellyellow}'
                elif exp == 2:
                    cell_colors[i] = r'\cellcolor[HTML]{\cellorange}'
                elif exp > 2:
                    cell_colors[i] = r'\cellcolor[HTML]{\cellred}'
        for (value, separator, color) in zip(cell_values, cell_separators, cell_colors):
            print(f'\\multicolumn{{1}}{{c|}}{{{color}{value}}} {separator} ', end='')
        print()
else:
    for i, (mean, std) in enumerate(zip(means, stds)):
        np.savetxt(sys.stdout, mean.T, delimiter=",", fmt="%f") # sizes on rows, objectives on columns, as in the paper
        print()
        np.savetxt(sys.stdout, std.T, delimiter=",", fmt="%f") # sizes on rows, objectives on columns, as in the paper
        if (i + 1 != len(means)):
            print()
