// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string>   // std::stoull
#include <unistd.h> // getopt

// Modules
#include "env.hpp"
#include "types.hpp"
#include "chvi.hpp"

static inline void print_usage(const char *bin) {

    fmt::print(stderr, "Usage: {} [-h] [-d dimensions] [-n size] [-s seed] [-g goals] ", bin);
    fmt::print(stderr, "[-f discount_factor] [-i max_iterations] [-e epsilon] [-o] [-0]\n");
}

#define parameter(CHAR, VAR, PARSE, CONDITION) \
    case CHAR: \
        VAR = PARSE(optarg); \
        if (!(CONDITION)) { \
            fmt::print(stderr, "{}: invalid value for -{} = '{}'\n", argv[0], opt, optarg); \
            return EXIT_FAILURE; \
        } \
        continue

#define flag(CHAR, VAR, BOOL) \
    case CHAR: \
        VAR = BOOL; \
        continue

int main(int argc, char** argv) {

    // default parameters
    int dimensions = 5;
    int size = 5;
    std::size_t seed = 0;
    double discount_factor = 1;
    int max_iterations = 100;
    double epsilon = 0.05;
    bool output = false;
    bool only_initial_state = false;

    char opt;
    while ((opt = getopt(argc, argv, "d:n:s:g:f:i:e:o0h")) != -1) {
        switch (opt) {
            parameter('d', dimensions, std::stoi, dimensions >= 2);
            parameter('n', size, std::stoi, size >= 2);
            parameter('s', seed, std::stoull, true);
            parameter('f', discount_factor, std::stod, discount_factor > 0);
            parameter('i', max_iterations, std::stoi, max_iterations > 0);
            parameter('e', epsilon, std::stod, epsilon >= 0);
            flag('o', output, true);
            flag('0', only_initial_state, true);
            case 'h':
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    Env env {(std::size_t)dimensions, (std::size_t)size, seed};
    const auto V = run_chvi(env, discount_factor, max_iterations, epsilon, !(output || only_initial_state));

    if (output) {
        fmt::print("{}\n", V);
    }

    if (only_initial_state) {
        fmt::print("{}\n", V[0]);
    }

    return EXIT_SUCCESS;
}
