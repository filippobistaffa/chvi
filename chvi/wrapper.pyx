#cython: language_level=3

import gymnasium as gym

cpdef void run(env: gym.Env):
    print(f"Convex Hull Value Iteration")
    print(f"Environment: {env}")
