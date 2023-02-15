#ifndef PAGMO_HPP_
#define PAGMO_HPP_

// Code extracted from https://github.com/esa/pagmo2 without modifications,
// apart from pareto_dominance() routine, which was adapted for maximization

#include <stdexcept>
#include <type_traits>
#include <cmath>
#include <tuple>
#include <vector>

namespace pagmo
{

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <typename... Args>
using conjunction = std::conjunction<Args...>;

template <typename... Args>
using disjunction = std::disjunction<Args...>;

typedef std::vector<double> vector_double;
typedef std::vector<vector_double>::size_type pop_size_t;

namespace detail
{

template <typename Exception>
struct ex_thrower {
    // Determine the type of the __LINE__ macro.
    using line_type = std::decay_t<decltype(__LINE__)>;
    explicit ex_thrower(const char *file, line_type line, const char *func) : m_file(file), m_line(line), m_func(func)
    {
    }
    template <typename... Args, enable_if_t<std::is_constructible<Exception, Args...>::value, int> = 0>
    [[noreturn]] void operator()(Args &&...args) const
    {
        throw Exception(std::forward<Args>(args)...);
    }
    template <typename Str, typename... Args,
              enable_if_t<conjunction<std::is_constructible<Exception, std::string, Args...>,
                                      disjunction<std::is_same<std::decay_t<Str>, std::string>,
                                                  std::is_same<std::decay_t<Str>, char *>,
                                                  std::is_same<std::decay_t<Str>, const char *>>>::value,
                          int> = 0>
    [[noreturn]] void operator()(Str &&desc, Args &&...args) const
    {
        std::string msg("\nfunction: ");
        msg += m_func;
        msg += "\nwhere: ";
        msg += m_file;
        msg += ", ";
        msg += std::to_string(m_line);
        msg += "\nwhat: ";
        msg += desc;
        msg += "\n";
        throw Exception(std::move(msg), std::forward<Args>(args)...);
    }
    const char *m_file;
    const line_type m_line;
    const char *m_func;
};

template <typename T, bool After = true>
inline bool greater_than_f(T a, T b)
{
    static_assert(std::is_floating_point<T>::value, "greater_than_f can be used only with floating-point types.");
    if (!std::isnan(a)) {
        if (!std::isnan(b))
            return a > b; // a > b
        else
            return !After; // a > nan
    } else {
        if (!std::isnan(b))
            return After; // nan > b
        else
            return false; // nan > nan
    }
}

template <typename T, bool After = true>
inline bool less_than_f(T a, T b)
{
    static_assert(std::is_floating_point<T>::value, "less_than_f can be used only with floating-point types.");
    if (!std::isnan(a)) {
        if (!std::isnan(b))
            return a < b; // a < b
        else
            return After; // a < nan
    } else {
        if (!std::isnan(b))
            return !After; // nan < b
        else
            return false; // nan < nan
    }
}

} // end namespace detail

#define pagmo_throw(exception_type, ...)                                                                               \
    pagmo::detail::ex_thrower<exception_type>(__FILE__, __LINE__, __func__)(__VA_ARGS__)

using fnds_return_type = std::tuple<std::vector<std::vector<pop_size_t>>, std::vector<std::vector<pop_size_t>>,
                                    std::vector<pop_size_t>, std::vector<pop_size_t>>;

bool pareto_dominance(const vector_double &obj1, const vector_double &obj2)
{
    if (obj1.size() != obj2.size()) {
        pagmo_throw(std::invalid_argument,
                    "Different number of objectives found in input fitnesses: " + std::to_string(obj1.size()) + " and "
                        + std::to_string(obj2.size()) + ". I cannot define dominance");
    }
    bool found_strictly_dominating_dimension = false;
    for (decltype(obj1.size()) i = 0u; i < obj1.size(); ++i) {
        if (detail::less_than_f(obj1[i], obj2[i])) {
            return false;
        } else if (detail::greater_than_f(obj1[i], obj2[i])) {
            found_strictly_dominating_dimension = true;
        }
    }
    return found_strictly_dominating_dimension;
}

fnds_return_type fast_non_dominated_sorting(const std::vector<vector_double> &points)
{
    auto N = points.size();
    // We make sure to have two points at least (one could also be allowed)
    if (N < 2u) {
        pagmo_throw(std::invalid_argument, "At least two points are needed for fast_non_dominated_sorting: "
                                               + std::to_string(N) + " detected.");
    }
    // Initialize the return values
    std::vector<std::vector<pop_size_t>> non_dom_fronts(1u);
    std::vector<std::vector<pop_size_t>> dom_list(N);
    std::vector<pop_size_t> dom_count(N);
    std::vector<pop_size_t> non_dom_rank(N);

    // Start the fast non dominated sort algorithm
    for (decltype(N) i = 0u; i < N; ++i) {
        dom_list[i].clear();
        dom_count[i] = 0u;
        for (decltype(N) j = 0u; j < i; ++j) {
            if (pareto_dominance(points[i], points[j])) {
                dom_list[i].push_back(j);
                ++dom_count[j];
            } else if (pareto_dominance(points[j], points[i])) {
                dom_list[j].push_back(i);
                ++dom_count[i];
            }
        }
    }
    for (decltype(N) i = 0u; i < N; ++i) {
        if (dom_count[i] == 0u) {
            non_dom_rank[i] = 0u;
            non_dom_fronts[0].push_back(i);
        }
    }
    // we copy dom_count as we want to output its value at this point
    auto dom_count_copy(dom_count);
    auto current_front = non_dom_fronts[0];
    std::vector<std::vector<pop_size_t>>::size_type front_counter(0u);
    while (current_front.size() != 0u) {
        std::vector<pop_size_t> next_front;
        for (decltype(current_front.size()) p = 0u; p < current_front.size(); ++p) {
            for (decltype(dom_list[current_front[p]].size()) q = 0u; q < dom_list[current_front[p]].size(); ++q) {
                --dom_count_copy[dom_list[current_front[p]][q]];
                if (dom_count_copy[dom_list[current_front[p]][q]] == 0u) {
                    non_dom_rank[dom_list[current_front[p]][q]] = front_counter + 1u;
                    next_front.push_back(dom_list[current_front[p]][q]);
                }
            }
        }
        ++front_counter;
        current_front = next_front;
        if (current_front.size() != 0u) {
            non_dom_fronts.push_back(current_front);
        }
    }
    return std::make_tuple(std::move(non_dom_fronts), std::move(dom_list), std::move(dom_count),
                           std::move(non_dom_rank));
}

} // end namespace pagmo

#endif
