import multiprocessing as mp
import numpy as np
import time

from manel_convexhull import get_hull, translate_hull
from manel_walkroom import WalkRoom


def Q_function_calculator(env, state, V, discount_factor):
    state_v = env.state_scalar_to_vector(state)
    #print(f"State {state_v}")
    hulls = list()
    for action in range(env.action_space.n):
        env.reset(state_v)
        next_state, rewards, d, _ = env.step(action)
        #print(f"Executed action {action} on state {state_v} (id: {state}) -> new state: {next_state} rewards: {rewards}")
        next_state_sc = env.state_vector_to_scalar(next_state)
        V_state = V[next_state_sc].copy()
        hull_sa = translate_hull(rewards, discount_factor, V_state)
        #print("transformed", hull_sa)
        for point in hull_sa:
            hulls.append(point)
    hulls = np.unique(np.array(hulls), axis=0)
    #print('points', hulls)
    new_hull = get_hull(hulls)
    return new_hull


def sweeping(start, end, env, V, discount_factor):
    #print("Start :", start, ". End :", end)
    new_V = [np.array([])] * int(end)
    for scalar_state in range(int(start), int(end)):
        if not env.is_terminal_scalar(scalar_state):
            new_V[scalar_state] = Q_function_calculator(env, scalar_state, V, discount_factor)
    return new_V[int(start):int(end)]


def partial_convex_hull_value_iteration(env, discount_factor=1.0, max_iterations=100, epsilon=0.01, verbose=False):
    V = [np.array([])] * env.n_states
    iteration = 0
    previous_delta = 0
    while iteration < max_iterations:
        start_time = time.time()
        results = list()
        delta = 0
        iteration += 1
        if False: # Manel's parallel code
            num_procs = 4
            with mp.Pool(processes=6) as pool:
                for i in range(num_procs):
                    results.append(pool.apply_async(sweeping, args=(i*env.n_states/num_procs,(i+1)*env.n_states/num_procs, env, V, discount_factor)))
                #pool.apply_async(sweeping, args=(0, env.n_states, env, V, discount_factor), callback=resulter.collect_result)
                pool.close()
                pool.join()
            for i in range(num_procs):
                n_elements = 0
                for r in results:
                    for element in r.get():
                        V[n_elements] = element.copy()
                        delta += len(V[n_elements])
                        n_elements += 1
        V = sweeping(0, env.n_states, env, V, discount_factor)
        for hull in V:
            delta += len(hull)
        if verbose:
            print(f'V = {V}')
            print("Iterations: ", iteration, "/", max_iterations, ". Delta: ", (delta-previous_delta)/env.n_states)
            print("---")
        if abs(delta-previous_delta) / env.n_states <= epsilon:
            break
        previous_delta = delta
    return V
