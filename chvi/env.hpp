#ifndef ENV_HPP_
#define ENV_HPP_

#include <set>          // std::set
#include <tuple>        // std::make_tuple
#include <cmath>        // std::pow
#include "types.hpp"    // std::vector, coordinate type
#include "pgc.hpp"      // pseudo-random number generator

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

class Env {

    std::size_t dimensions;
    std::size_t size;
    std::set<std::vector<coordinate>> goals;

  public:
    std::vector<std::size_t> state_space_size;
    std::size_t action_space_size;
    std::size_t seed;

    Env(std::size_t dimensions, std::size_t size, std::size_t seed, double goal_percentage = 0.01):
        dimensions(dimensions), size(size), seed(seed) {

            state_space_size = std::vector<std::size_t>(dimensions, size);
            action_space_size = 2 * dimensions;
            const auto n_goals = std::max(1ULL, (unsigned long long)(goal_percentage * std::pow(size, dimensions)));
            // use our 64-bit seed to get 2 32-bit seeds needed by this PRNG
            const uint32_t upper_seed = seed >> 32;
            const uint32_t lower_seed = seed;
            // initialize PRNG
            auto rng = pcg32_srandom_r(upper_seed, lower_seed);

            for (std::size_t i = 0; i < n_goals; ++i) {
                std::vector<coordinate> p(dimensions);
                do {
                    for (std::size_t dimension = 0; dimension < dimensions; ++dimension) {
                        const auto random = pcg32_random_r(rng);
                        p[dimension] = random % size;
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

    auto execute_action(std::vector<coordinate> state, std::size_t action) {

        const auto dimension = action / 2;
        const auto step = 2.0 * (action % 2) - 1;
        state[dimension] = std::clamp(state[dimension] + step, 0.0, (double)size - 1);
        std::vector<coordinate> rw(state.size(), 0);
        rw[dimension] = -1;
        return std::make_tuple(state, rw);
    }

    bool is_terminal(std::vector<coordinate> state) {

        return (goals.contains(state));
    }
};

#endif
