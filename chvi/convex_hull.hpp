#ifndef CONVEX_HULL_HPP_
#define CONVEX_HULL_HPP_

#include "types.hpp"            // point type
#include <set>                  // std::set
#include <algorithm>            // std::transform
#include <libqhullcpp/Qhull.h>  // qhull library
#include "pagmo.hpp"            // code extracted from pagmo library

auto scale_points(const std::vector<point> &points, const double gamma) {

    std::vector<point> scaled(points.size());

    std::transform(std::begin(points), std::end(points), std::begin(scaled), [&gamma](const point &p) {
        point sp(p.size());
        std::transform(std::begin(p), std::end(p), std::begin(sp), [&gamma](coordinate c) {
            return gamma * c;
        });
        return sp;
    });

    return scaled;
}

auto transpose_points(const std::vector<point> &points, const point &delta) {

    std::vector<point> transposed(points.size());

    std::transform(std::begin(points), std::end(points), std::begin(transposed), [&delta](const point &p) {
        point tp(p.size());
        std::transform(std::begin(p), std::end(p), std::begin(delta), std::begin(tp), std::plus<coordinate>());
        return tp;
    });

    return transposed;
}

auto linear_transformation(const std::vector<point> &points, const double gamma, const point &delta) {

    return transpose_points(scale_points(points, gamma), delta);
}

auto non_dominated(const std::vector<point> &points) {

    // https://esa.github.io/pagmo2/docs/cpp/utils/multi_objective.html#namespacepagmo_1a27aeb5efb01fca4422fc124eec221199
    // See "A Fast Elitist Non-dominated Sorting Genetic Algorithm for Multi-objective Optimization: NSGA-II" in "pdf" folder
    // Points need to be scaled by -1 since pagmo library assumes minimization
    auto [ non_dom_fronts, dom_list, dom_count, non_dom_rank ] = pagmo::fast_non_dominated_sorting(scale_points(points, -1));

    // compile output
    const auto pareto = non_dom_fronts.front(); // containts points' indices with respect to input
    std::vector<point> non_dominated(pareto.size());
    std::transform(std::begin(pareto), std::end(pareto), std::begin(non_dominated), [&points](const auto &i) {
        return points[i];
    });

    return non_dominated;
}

template<typename T>
auto convex_hull(const T &points) {

    std::vector<point> convex_hull;

    // check for empty input set of points
    if (points.size() == 0) {
        return convex_hull;
    }

    // compute number of dimensions
    const auto dimensions = std::begin(points)->size();

    // compile input for qhull
    std::vector<coordinate> coordinates;
    for (const auto &p : points) {
        coordinates.insert(std::end(coordinates), std::begin(p), std::end(p));
    }

    try {

        // try to compute convex hull
        orgQhull::Qhull qhull;
        const char *input_commands = "";
        const char *qhull_commands = "";
        qhull.runQhull(input_commands, dimensions, points.size(), coordinates.data(), qhull_commands);

        // compile output
        orgQhull::QhullVertexList vertices = qhull.vertexList();
        for (const orgQhull::QhullVertex &vertex : vertices) {
            coordinate *coordinates = vertex.point().coordinates();
            point p(coordinates, coordinates + dimensions);
            convex_hull.push_back(p);
        }

    } catch (orgQhull::QhullError &e) {

        // in case of error return the input set of points
        convex_hull.insert(std::end(convex_hull), std::begin(points), std::end(points));

    }

    return convex_hull;
}

#endif
