#ifndef CHVI_HPP_
#define CHVI_HPP_

#include "types.hpp" // coordinate type
#include <vector>    // std::vector

#ifdef CYTHON

#include "Python.h"
typedef PyObject *env_type;

#else

#include <tuple>    // std::tuple
#include "env.hpp"  // Env type
typedef Env &env_type;

inline auto execute_action(env_type env, const std::vector<coordinate> &state, const std::size_t action) {

    return env.execute_action(state, action);
}

inline auto get_action_space_size(env_type env) {

    return env.action_space_size;
}

inline auto get_observation_space_size(env_type env) {

    return env.state_space_size;
}

inline auto get_n_goals(env_type env) {

    return env.n_goals;
}

inline auto is_terminal(env_type env, std::vector<coordinate> state) {

    return env.is_terminal(state);
}

#endif

std::vector<std::vector<coordinate>> run_chvi(env_type env, const double discount_factor, const std::size_t max_iterations, const double epsilon = 0, const bool verbose = true);

#endif
