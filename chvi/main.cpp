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
    fmt::print(stderr, "[-f discount_factor] [-i max_iterations] [-e epsilon] [-o]\n");
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
    double goals = 0.01;
    double discount_factor = 1;
    int max_iterations = 100;
    double epsilon = 0.05;
    bool output = false;

    char opt;
    while ((opt = getopt(argc, argv, "d:n:s:g:f:i:e:oh")) != -1) {
        switch (opt) {
            parameter('d', dimensions, std::stoi, dimensions >= 2);
            parameter('n', size, std::stoi, size >= 2);
            parameter('s', seed, std::stoull, true);
            parameter('g', goals, std::stod, goals > 0 && goals <= 1);
            parameter('f', discount_factor, std::stod, discount_factor > 0);
            parameter('i', max_iterations, std::stoi, max_iterations > 0);
            parameter('e', epsilon, std::stod, epsilon >= 0);
            flag('o', output, true);
            case 'h':
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    Env env {(std::size_t)dimensions, (std::size_t)size, seed, goals};
    const auto V = run_chvi(env, discount_factor, max_iterations, epsilon, !output);

    if (output) {
        fmt::print("{}\n", V);
    }

    return EXIT_SUCCESS;
}
