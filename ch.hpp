#ifndef CH_HPP_
#define CH_HPP_

#include <vector>       // std::vector
#include "types.hpp"    // point type

std::vector<point> non_dominated(const std::vector<point> &points);

std::vector<point> convex_hull(const std::vector<point> &points);

#endif
