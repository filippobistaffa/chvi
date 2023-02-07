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

void test_chvi() {

    fmt::print("We're now inside C++\n");

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
}
