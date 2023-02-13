// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string>   // std::stoull

// Modules
#include "env.hpp"
#include "types.hpp"
#include "chvi.hpp"

int main(int argc, char** argv) {

    std::size_t arg = 1;
    std::size_t dimensions = std::stoull(argv[arg++]);
    std::vector<std::size_t> state_space_size;
    while (dimensions-- != 0) {
        state_space_size.push_back(std::stoull(argv[arg++]));
    }
    const std::size_t actions = std::stoull(argv[arg++]);
    const std::size_t seed = std::stoull(argv[arg++]);
    const double discount_factor = std::stod(argv[arg++]);
    const std::size_t max_iterations = std::stoull(argv[arg++]);
    const double epsilon = std::stod(argv[arg++]);

    Env env {state_space_size, actions, seed};
    fmt::print("{}\n", run_chvi(env, discount_factor, max_iterations, epsilon, false));

    return EXIT_SUCCESS * argc;
}
