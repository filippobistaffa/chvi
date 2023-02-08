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

std::size_t cpp_chvi(PyObject *env) {

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

    log_line();
    log_title("Convex Hull Value Iteration");
    log_line();
    log_value("Observation space size", fmt::format("{}", observation_space_size(env)));
    log_value("Action space size", action_space_size(env));
    log_line();

    return execute_action(env, {1, 2}, 2);
}
