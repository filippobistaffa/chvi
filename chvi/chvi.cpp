#include "chvi.hpp"

#include <numeric>  // std::accumulate, std::partial_sum

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>
#define LABEL_WIDTH 20

// modules
#include "types.hpp"
#include <wrapper.h>
#include "convex_hull.hpp"
#include "log.hpp"

auto id2vector(const std::size_t id, const std::vector<std::size_t> &pfx_product, const std::vector<std::size_t> &state_space_size) {

    std::vector<std::size_t> state(state_space_size.size());
    for (std::size_t dimension = 0; dimension < state_space_size.size(); ++dimension) {
        state[dimension] = (id / pfx_product[dimension]) % state_space_size[dimension];
    }
    return state;
}

void run_chvi(PyObject *env, double discount_factor, std::size_t max_iterations) {

    /*
    std::vector<point> points{
        {0, 0, 4},
        {0, 5, 3},
        {1, 7, 0},
        {2, 1, 4},
        {3, 4, 5},
        {4, 2, 3},
        {4, 4, 6},
        {4, 6, 7},
        {5, 0, 2},
        {6, 4, 1},
        {6, 5, 1},
        {6, 7, 0},
        {7, 4, 3}
    };

    for (const auto &p : points) {
        fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Input Point", p);
    }
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Negated points", translate_hull(points, -1, {0, 0, 0}));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Shifted points", translate_hull(points, 1, {1, 2, 1}));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Convex hull", convex_hull(points));
    fmt::print("{1:<{0}} {2}\n", LABEL_WIDTH, "Non-dominated", non_dominated(points));
    */

    const auto state_space_size = get_observation_space_size(env);
    const auto n_states = std::accumulate(std::begin(state_space_size), std::end(state_space_size), 1ULL, std::multiplies<>());
    const auto action_space_size = get_action_space_size(env);

    log_line();
    log_title("Convex Hull Value Iteration");
    log_line();
    log_value("State space size", fmt::format("{}", state_space_size));
    log_value("Number of states", fmt::format("{}", n_states));
    log_value("Action space size", fmt::format("{}", action_space_size));
    log_line();

    // data structure useful to associate each thread to a vector state
    std::vector<std::size_t> pfx_product(state_space_size.size(), 1ULL);
    std::partial_sum(std::begin(state_space_size), std::end(state_space_size) - 1, std::begin(pfx_product) + 1, std::multiplies<>());

    for (std::size_t thread = 0; thread < n_states; ++thread) {
        auto state = id2vector(thread, pfx_product, state_space_size);
        fmt::print("thread {:>2} -> {}\n", thread, state);
    }
}
