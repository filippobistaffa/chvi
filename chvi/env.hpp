#ifndef ENV_HPP_
#define ENV_HPP_

#include <cmath>        // std::pow
#include <numeric>      // std::iota
#include <tuple>        // std::make_tuple
#include <numeric>      // std::accumulate, std::partial_sum
#include "types.hpp"    // std::vector, point type

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

class Env {

    std::vector<std::size_t> ex_pfx_product;
    std::size_t n_states;

  public:
    std::vector<std::size_t> state_space_size;
    std::size_t action_space_size;
    std::size_t seed;

    Env(std::vector<std::size_t> state_space_size, std::size_t action_space_size, std::size_t seed):
        state_space_size(state_space_size), action_space_size(action_space_size), seed(seed) {
            ex_pfx_product = std::vector<std::size_t>(state_space_size.size(), 1ULL);
            std::partial_sum(std::begin(state_space_size), std::end(state_space_size) - 1, std::begin(ex_pfx_product) + 1, std::multiplies<>());
            std::accumulate(std::begin(state_space_size), std::end(state_space_size), 1ULL, std::multiplies<>());
        }

    auto state2id(const point &state) {

        std::size_t id = 0;

        for (std::size_t dimension = 0; dimension < state.size(); ++dimension) {
            id += state[dimension] * ex_pfx_product[dimension];
        }

        return id;
    }

    auto id2state(const std::size_t id) {

        point state(state_space_size.size());

        for (std::size_t dimension = 0; dimension < state_space_size.size(); ++dimension) {
            state[dimension] = (id / ex_pfx_product[dimension]) % state_space_size[dimension];
        }

        return state;
    }

    auto random(std::size_t seed) {

        constexpr std::size_t a = 1103515245;
        constexpr std::size_t c = 12345;
        constexpr std::size_t m = 1ULL << 31;
        seed = (a * seed + c) % m;
        return seed;
    }

    auto execute_action(point state, const std::size_t action) {

        point rw(state.size());
        std::transform(std::begin(state), std::end(state), std::begin(rw), [&action](coordinate c) {
            return c + action;
        });
        //fmt::print("old state = {}\n", state);
        state = id2state(random(state2id(state)) % n_states);
        //fmt::print("new state = {}\n", state);
        return std::make_tuple(state, rw);
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
