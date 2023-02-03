#include "ch.hpp"

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/ranges.h>

// qhull library
#include <libqhullcpp/Qhull.h>

std::set<point> convex_hull(const std::set<point> &points) {

    // compile input for qhull
    const int dimensions = std::begin(points)->size();
    std::vector<coordinate> coordinates;
    for (auto &p : points) {
        //fmt::print("Point: {}\n", p);
        coordinates.insert(std::end(coordinates), std::begin(p), std::end(p));
    }
    //fmt::print("Coordinates: {}\n", coordinates);

    // compute convex hull
    orgQhull::Qhull qhull;
    const char *input_commands = "";
    const char *qhull_commands = "";
    qhull.runQhull(input_commands, dimensions, points.size(), coordinates.data(), qhull_commands);

    // compile output
    orgQhull::QhullVertexList vertices = qhull.vertexList();
    std::set<point> convex_hull;
    for (orgQhull::QhullVertex &vertex : vertices) {
        coordinate *coordinates = vertex.point().coordinates();
        point p(coordinates, coordinates + dimensions);
        //fmt::print("Point: {}\n", p);
        convex_hull.insert(p);
    }

    return convex_hull;
}
