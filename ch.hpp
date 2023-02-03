#ifndef CH_HPP_
#define CH_HPP_

#include <set>          // std::set
#include "types.hpp"    // point type

std::set<point> convex_hull(const std::set<point> &points);

#endif
