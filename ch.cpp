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

    // https://esa.github.io/pagmo2/docs/cpp/utils/multi_objective.html#namespacepagmo_1a27aeb5efb01fca4422fc124eec221199
    // See "A Fast Elitist Non-dominated Sorting Genetic Algorithm for Multi-objective Optimization: NSGA-II" in "pdf" folder
    auto [ non_dom_fronts, dom_list, dom_count, non_dom_rank ] = pagmo::fast_non_dominated_sorting(points);
    fmt::print("{:<25} {}\n", "Non-dominated fronts:", non_dom_fronts);
    fmt::print("{:<25} {}\n", "Domination list:", dom_list);
    fmt::print("{:<25} {}\n", "Domination count:", dom_count);
    fmt::print("{:<25} {}\n", "Non-domination rank:", non_dom_rank);

    std::vector<point> non_dominated;
    return non_dominated;
}

std::vector<point> convex_hull(const std::vector<point> &points) {

    // compile input for qhull
    std::vector<coordinate> coordinates;
    for (auto &p : points) {
        fmt::print("{:<25} {}\n", "Input Point:", p);
        coordinates.insert(std::end(coordinates), std::begin(p), std::end(p));
    }

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
        fmt::print("{:<25} {}\n", "Convex Hull Point:", p);
        convex_hull.push_back(p);
    }

    return convex_hull;
}
