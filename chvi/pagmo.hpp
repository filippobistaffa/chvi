#ifndef PAGMO_HPP_
#define PAGMO_HPP_

#include <vector>

namespace pagmo
{

typedef std::vector<double> vector_double;
typedef std::vector<vector_double>::size_type pop_size_t;
using fnds_return_type = std::tuple<std::vector<std::vector<pop_size_t>>, std::vector<std::vector<pop_size_t>>,
                                    std::vector<pop_size_t>, std::vector<pop_size_t>>;

fnds_return_type fast_non_dominated_sorting(const std::vector<vector_double> &);

}

#endif
