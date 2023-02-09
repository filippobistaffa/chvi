#ifndef CONVEX_HULL_HPP_
#define CONVEX_HULL_HPP_

#include "types.hpp"    // point type

std::vector<point> translate_hull(const std::vector<point> &points, const double gamma, const point &delta);

std::vector<point> non_dominated(const std::vector<point> &points);

std::vector<point> convex_hull(const std::vector<point> &points);

#endif
