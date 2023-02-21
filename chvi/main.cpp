// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string>   // std::stoull

// Modules
#include "env.hpp"
#include "types.hpp"
#include "chvi.hpp"

int main([[ maybe_unused ]] int argc, char** argv) {

    int arg = 1;
    const std::size_t dimensions = std::stoull(argv[arg++]);
    const std::size_t size = std::stoull(argv[arg++]);
    const std::size_t seed = std::stoull(argv[arg++]);
    const double goals = std::stod(argv[arg++]);
    const double discount_factor = std::stod(argv[arg++]);
    const std::size_t max_iterations = std::stoull(argv[arg++]);
    const double epsilon = std::stod(argv[arg++]);
    bool verbose = arg == argc || strcmp(argv[arg], "--output");

    Env env {dimensions, size, seed, goals};
    const auto V = run_chvi(env, discount_factor, max_iterations, epsilon, verbose);

    if (!verbose) {
        fmt::print("{}\n", V);
    }

    return EXIT_SUCCESS;
}
