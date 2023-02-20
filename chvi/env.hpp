#ifndef ENV_HPP_
#define ENV_HPP_

#include <set>          // std::set
#include <tuple>        // std::make_tuple
#include <cmath>        // std::pow
#include "types.hpp"    // std::vector, point type

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

class Env {

    std::size_t dimensions;
    std::size_t size;
    std::set<point> goals;

    static auto random(std::size_t seed) {

        constexpr std::size_t a = 1103515245;
        constexpr std::size_t c = 12345;
        constexpr std::size_t m = 1ULL << 31;
        return (a * seed + c) % m;
    }

  public:
    std::vector<std::size_t> state_space_size;
    std::size_t action_space_size;
    std::size_t seed;

    Env(std::size_t dimensions, std::size_t size, std::size_t seed, double goal_percentage = 0.01):
        dimensions(dimensions), size(size), seed(seed) {
    
            state_space_size = std::vector<std::size_t>(dimensions, size);
            action_space_size = 2 * dimensions;
            const size_t n_goals = goal_percentage * std::pow(size, dimensions);

            for (std::size_t i = 0; i < n_goals; ++i) {
                point p(dimensions);
                do {
                    for (std::size_t dimension = 0; dimension < dimensions; ++dimension) {
                        seed = random(seed);
                        p[dimension] = seed % size;
                    }
                } while (
                    // if the point is the origin or it is already in the set, repeat the generation
                    std::all_of(std::begin(p), std::end(p), [](coordinate s) { return s == 0; })
                    || goals.contains(p)
                );
                goals.insert(p);
            }
            //fmt::print("{}\n", n_goals);
            //fmt::print("{}\n", goals);
        }
    
    auto execute_action(point state, std::size_t action) {

        const auto dimension = action / 2;
        const auto step = 2.0 * (action % 2) - 1;
        state[dimension] = std::clamp(state[dimension] + step, 0.0, (double)size - 1);
        point rw(state.size(), 0);
        rw[dimension] = -1;
        return std::make_tuple(state, rw);
    }

    bool is_terminal(point state) {

        return (goals.contains(state));
    }
};

#endif
