// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string>   // std::stoull

// Modules
#include "env.hpp"
#include "types.hpp"
#include "convex_hull.hpp"

int main(int argc, char** argv) {

    std::size_t arg = 1;
    std::size_t dimensions = std::stoull(argv[arg++]);
    std::vector<std::size_t> state_space_size;
    while (dimensions-- != 0) {
        state_space_size.push_back(std::stoull(argv[arg++]));
    }
    std::size_t actions = std::stoull(argv[arg++]);
    std::size_t seed = std::stoull(argv[arg++]);
    std::size_t steps = std::stoull(argv[arg++]);

    Env env{ state_space_size, actions, seed};
    std::vector<std::tuple<point, point, bool>> sequence;

    for (std::size_t step = 0; step < steps; ++step) {
        sequence.push_back(env.step(step));
    }

    fmt::print("{}\n", sequence);
    return EXIT_SUCCESS * argc;
}
