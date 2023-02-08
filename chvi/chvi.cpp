#include "chvi.hpp"

#include "types.hpp"
#include <wrapper.h>
#include "convex_hull.hpp"
#include "log.hpp"

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>
#define LABEL_WIDTH 20

#include <random>

void cpp_chvi(PyObject *env) {

    /*
    std::vector<point> points{
        {0, 0, 4},
        {0, 5, 3},
        {1, 7, 0},
        {2, 1, 4},
        {3, 4, 5},
        {4, 2, 3},
        {4, 4, 6},
        {4, 6, 7},
        {5, 0, 2},
        {6, 4, 1},
        {6, 5, 1},
        {6, 7, 0},
        {7, 4, 3}
    };

    for (const auto &p : points) {
        fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Input Point", p);
    }
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Negated points", translate_hull(points, -1, {0, 0, 0}));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Shifted points", translate_hull(points, 1, {1, 2, 1}));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Convex hull", convex_hull(points));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Non-dominated", non_dominated(points));
    */

    log_line();
    log_title("Convex Hull Value Iteration");
    log_line();
    log_value("Observation space size", fmt::format("{}", get_observation_space_size(env)));
    log_value("Action space size", fmt::format("{}", get_action_space_size(env)));
    log_value("Initial state", fmt::format("{}", get_state(env)));
    log_line();

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> uniform(0, get_action_space_size(env) - 1);
    int iterations = 20;

    while (iterations--) {
        const auto state = get_state(env);
        const auto action = uniform(gen);
        const auto rewards = get_action_rewards(env, state, action);
        log_value(fmt::format("Reward of action {}", action), fmt::format("{}", rewards));
        log_value("Current state", fmt::format("{}", state));
    }

    log_line();

    return ;
}
