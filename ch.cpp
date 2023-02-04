#include "ch.hpp"

#include <iostream>
#include <limits>   // std::numeric_limits<double>::infinity()

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/ranges.h>

// qhull library
#include <libqhullcpp/Qhull.h>

// pagmo library
#include <pagmo/utils/multi_objective.hpp>

std::vector<point> non_dominated(const std::vector<point> &points) {

    // "A Fast Elitist Non-dominated Sorting Genetic Algorithm for Multi-objective Optimization: NSGA-II"
    // See "pdf" folder
    auto [ non_dom_fronts, dom_list, dom_count, non_dom_rank ] = pagmo::fast_non_dominated_sorting(points);
    fmt::print("Non-dominated fronts:\t{}\n", non_dom_fronts);
    fmt::print("Domination list:\t{}\n", dom_list);
    fmt::print("Domination count:\t{}\n", dom_count);
    fmt::print("Non-domination rank:\t{}\n", non_dom_rank);

    std::vector<point> non_dominated;
    return non_dominated;
}

std::vector<point> convex_hull(const std::vector<point> &points) {

    // compile input for qhull
    std::vector<coordinate> coordinates;
    for (auto &p : points) {
        fmt::print("Point: {}\n", p);
        coordinates.insert(std::end(coordinates), std::begin(p), std::end(p));
    }
    fmt::print("Coordinates: {}\n", coordinates);

    // compute convex hull
    orgQhull::Qhull qhull;
    const char *input_commands = "";
    const char *qhull_commands = "";
    qhull.runQhull(input_commands, DIMENSIONS, points.size(), coordinates.data(), qhull_commands);

    // compile output
    orgQhull::QhullVertexList vertices = qhull.vertexList();
    std::vector<point> convex_hull;
    for (orgQhull::QhullVertex &vertex : vertices) {
        coordinate *coordinates = vertex.point().coordinates();
        point p(coordinates, coordinates + DIMENSIONS);
        fmt::print("Point: {}\n", p);
        convex_hull.push_back(p);
    }

    return convex_hull;
}
