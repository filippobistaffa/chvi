#ifndef ENV_HPP_
#define ENV_HPP_

#include <tuple>        // std::make_tuple
#include "types.hpp"    // std::vector, point type

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

class Env {

    std::size_t dimensions;
    std::size_t size;

  public:
    std::vector<std::size_t> state_space_size;
    std::size_t action_space_size;
    std::size_t seed;

    Env(std::size_t dimensions, std::size_t size, std::size_t seed):
        dimensions(dimensions), size(size), seed(seed) {
    
            state_space_size = std::vector<std::size_t>(dimensions, size);
            action_space_size = 2 * dimensions;
        }

    auto random(std::size_t seed) {

        constexpr std::size_t a = 1103515245;
        constexpr std::size_t c = 12345;
        constexpr std::size_t m = 1ULL << 31;

        return (a * seed + c) % m;
    }

    auto execute_action(point state, const std::size_t action) {

        const auto dimension = action / 2;
        const auto step = 2.0 * (action % 2) - 1;
        state[dimension] = std::clamp(state[dimension] + step, 0.0, (double)size - 1);
        point rw(state.size(), 0);
        rw[dimension] = -1;

        return std::make_tuple(state, rw);
    }

    bool is_terminal(point state) {

        std::transform(std::begin(state), std::end(state), std::begin(state_space_size), std::begin(state), [](coordinate s, coordinate l) {
            return l - 1 - s;
        });

        return std::any_of(std::begin(state), std::end(state), [](coordinate s) {
            return s == 0;
        });
    }
};

#endif
