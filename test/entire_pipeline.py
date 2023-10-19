#!/usr/bin/python3

from subprocess import PIPE
import subprocess

import argparse as ap
import numpy as np

import time
import sys
import os

from parameters import parameters
from manel_step_2 import lex_max
from manel_step_3 import minimal_weight_computation


def list_of_lists(ch0, d):
    return [list(p) for p in np.array_split(ch0, len(ch0) / d)]


if __name__ == "__main__":

    parser = ap.ArgumentParser()
    parser.add_argument('--dimensions', type=int, default=2)
    parser.add_argument('--size', type=int, default=2)
    parser.add_argument('--seed', type=int, default=0)
    args = parser.parse_args()

    # STEP 1

    start_time = time.time()
    exe_subdir = 'chvi'
    exe_name = 'chvi'
    exe_abs_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), exe_subdir, exe_name)
    command_line = [exe_abs_path]
    command_line.extend([
        f'-d {args.dimensions}',
        f'-n {args.size}',
        f'-s {args.seed}',
        f'-f {parameters["discount_factor"]}',
        f'-i {parameters["max_iterations"]}',
        f'-e {parameters["epsilon"]}',
        '-o'
    ])
    output = subprocess.run(command_line, check=True, stdout=PIPE, stderr=PIPE).stdout.decode().rstrip()
    exec(f'convex_hulls = {output}')
    hull = list_of_lists(convex_hulls[0], args.dimensions)
    t1 = time.time() - start_time
    #print(hull)

    # STEP 2

    start_time = time.time()
    lex_index = lex_max(hull)
    t2 = time.time() - start_time
    #print(lex_index)

    # STEP 3

    start_time = time.time()
    weights = minimal_weight_computation(hull, lex_index)
    t3 = time.time() - start_time
    #print(weights)

    # PRINT RUNTIMES

    print(f'{args.dimensions},{args.size},{args.seed},{t1},{t2},{t3}')
