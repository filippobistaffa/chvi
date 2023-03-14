#ifndef CONVEX_HULL_HPP_
#define CONVEX_HULL_HPP_

#include "types.hpp"                    // coordinate type
#include <set>                          // std::set
#include <algorithm>                    // std::transform
#include <libqhullcpp/Qhull.h>          // qhull library
#include <libqhullcpp/QhullFacetList.h> // qhull library
#include <libqhullcpp/QhullVertexSet.h> // qhull library
#include "pagmo.hpp"                    // code extracted from pagmo library

auto scale_coordinates(const std::vector<coordinate> &coordinates, const double gamma) {

    std::vector<coordinate> scaled(coordinates.size());

    for (std::size_t c = 0; c < coordinates.size(); ++c) {
        scaled[c] = coordinates[c] * gamma;
    }

    return scaled;
}

auto transpose_coordinates(const std::vector<coordinate> &coordinates, const std::vector<coordinate> &delta) {

    std::vector<coordinate> transposed(coordinates.size());

    for (std::size_t p = 0; p < coordinates.size() / delta.size(); ++p) {
        for (std::size_t c = 0; c < delta.size(); ++c) {
            transposed[p * delta.size() + c] = coordinates[p * delta.size() + c] + delta[c];
        }
    }

    return transposed;
}

auto linear_transformation(const std::vector<coordinate> &coordinates, const double gamma, const std::vector<coordinate> &delta) {

    if (coordinates.size() == 0) {
        return std::vector<coordinate>(delta);
    }

    return transpose_coordinates(scale_coordinates(coordinates, gamma), delta);
}

auto non_dominated(const std::vector<std::vector<coordinate>> &points) {

    // check for empty input set of points
    if (points.size() == 0) {
        return std::vector<coordinate>();
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
    const auto dimensions = points.front().size();
    std::vector<coordinate> non_dominated(pareto.size() * dimensions);

    for (std::size_t p = 0; p < pareto.size(); ++p) {
        std::copy(std::begin(points[pareto[p]]), std::end(points[pareto[p]]), std::begin(non_dominated) + p * dimensions);
    }

    return non_dominated;
}

auto flatten(const std::vector<std::vector<coordinate>> &points) {

    const auto dimensions = points.front().size();
    std::vector<coordinate> flat(points.size() * dimensions);

    for (std::size_t p = 0; p < points.size(); ++p) {
        std::copy(std::begin(points[p]), std::end(points[p]), std::begin(flat) + p * dimensions);
    }

    return flat;
}

template<typename T>
auto convex_hull(const T &points) {

    std::vector<std::vector<coordinate>> convex_hull;

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
        std::set<std::vector<coordinate>> unique;
        for (const auto &facet : qhull.facetList()) {
            for (const auto &vertex : facet.vertices()) {
                const double *coordinates = vertex.point().coordinates();
                std::vector<coordinate> p(coordinates, coordinates + dimensions);
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
