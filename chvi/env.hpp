#ifndef CHVI_HPP_
#define CHVI_HPP_

#include <cmath>        // std::pow
#include <numeric>      // std::iota
#include <tuple>        // std::make_tuple
#include "types.hpp"    // std::vector, point type

class Env {

    std::vector<std::size_t> state_space_size;
    std::size_t action_space_size;
    std::size_t seed;
    point state;

  public:

    Env(std::vector<std::size_t> state_space_size, std::size_t action_space_size, std::size_t seed):
        state_space_size(state_space_size), action_space_size(action_space_size), seed(seed) {
            state = point(state_space_size.size(), 0);
        }

    auto step(std::size_t action) {

        point rw(state);
        std::vector<std::size_t> weights(state_space_size.size());
        std::iota(std::begin(weights), std::end(weights), 1ULL);

        for (std::size_t dimension = 0; dimension < state.size(); ++dimension) {
            state[dimension] = std::fmod(seed * weights[dimension] + state[dimension] * state[dimension], state_space_size[dimension]);
            rw[dimension] += action;
        }

        return std::make_tuple(state, rw, is_terminal(state));
    }

    bool is_terminal(point state) {

        bool terminal = false;

        for (std::size_t dimension = 0; dimension < state.size(); ++dimension) {
            terminal |= (state[dimension] + 1 == state_space_size[dimension]);
        }

        return terminal;
    }
};

#endif
