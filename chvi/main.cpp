// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>
#define LABEL_WIDTH 20

// Modules
#include "types.hpp"
#include "convex_hull.hpp"

int main() {

    std::vector<std::vector<double>> bad_points{
        {0, 0},
        {1, 1},
        {2, 2},
    };

    std::vector<std::vector<double>> good_points{
        {0, 0, 4},
        {0, 5, 3},
        {3, 4, 5},
        {4, 2, 3},
    };

    auto points = bad_points;

    for (const auto &p : points) {
        fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Input Point", p);
    }
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Negated points", linear_transformation(points, -1, {0, 0, 0}));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Shifted points", linear_transformation(points, 1, {1, 2, 1}));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Convex hull", convex_hull(points));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Non-dominated", non_dominated(points));
}
