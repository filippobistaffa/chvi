#include "ch.hpp"

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/ranges.h>

// qhull library
#include "libqhullcpp/Qhull.h"
//#include "libqhullcpp/QhullVertex.h"
//#include "libqhullcpp/QhullVertexSet.h"
//#include "libqhullcpp/QhullPoint.h"

std::set<point> convex_hull(const std::set<point> &points) {

    const int dimensions = std::begin(points)->size();
    std::vector<coordinate> coordinates;

    for (auto &point : points) {
        fmt::print("Point: {}\n", point);
        coordinates.insert(std::end(coordinates), std::begin(point), std::end(point));
    }

    fmt::print("Coordinates: {}\n", coordinates);

    orgQhull::Qhull qhull;
    const char *input_commands = "";
    const char *qhull_commands = "";
    qhull.runQhull(input_commands, dimensions, points.size(), coordinates.data(), qhull_commands);

    std::set<point> convex_hull;
    return convex_hull;
}
