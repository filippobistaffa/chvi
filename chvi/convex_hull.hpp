#ifndef CONVEX_HULL_HPP_
#define CONVEX_HULL_HPP_

#include "types.hpp"                    // point type
#include <set>                          // std::set
#include <algorithm>                    // std::transform
#include <libqhullcpp/Qhull.h>          // qhull library
#include <libqhullcpp/QhullFacetList.h> // qhull library
#include <libqhullcpp/QhullVertexSet.h> // qhull library
#include "pagmo.hpp"                    // code extracted from pagmo library

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

    if (points.size() == 0) {
        return std::vector<point>{ delta };
    }

    return transpose_points(scale_points(points, gamma), delta);
}

auto non_dominated(const std::vector<point> &points) {

    // check for empty input set of points
    if (points.size() <= 1) {
        return std::vector<point>(points);
    }

    // https://esa.github.io/pagmo2/docs/cpp/utils/multi_objective.html#namespacepagmo_1a27aeb5efb01fca4422fc124eec221199
    // See "A Fast Elitist Non-dominated Sorting Genetic Algorithm for Multi-objective Optimization: NSGA-II" in "pdf" folder
    // In contrast with the original pagmo code, this routine assumes maximization
    auto [ non_dom_fronts, dom_list, dom_count, non_dom_rank ] = pagmo::fast_non_dominated_sorting(points);

    // ignore some variables
    (void) dom_list;
    (void) dom_count;
    (void) non_dom_rank;

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

    // compile input for qhull (double type required by runQhull)
    std::vector<double> coordinates;
    for (const auto &p : points) {
        coordinates.insert(std::end(coordinates), std::begin(p), std::end(p));
    }

    try {

        // try to compute convex hull
        orgQhull::Qhull qhull;
        qhull.runQhull("", dimensions, points.size(), coordinates.data(), "Qx Qt");

        // iterate over all facets and, for each facet, over all vertices
        // maintain unique occurrences by means of a set data structure
        std::set<point> unique;
        for (const auto &facet : qhull.facetList()) {
            for (const auto &vertex : facet.vertices()) {
                const double *coordinates = vertex.point().coordinates();
                point p(coordinates, coordinates + dimensions);
                unique.insert(p);
            }
        }
        convex_hull.reserve(unique.size());
        convex_hull.assign(std::begin(unique), std::end(unique));

    } catch (orgQhull::QhullError &e) {

        // in case of error return the input set of points
        convex_hull.insert(std::end(convex_hull), std::begin(points), std::end(points));
    }

    return convex_hull;
}

#endif
