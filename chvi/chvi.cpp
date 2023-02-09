#include "chvi.hpp"

#include <numeric>  // std::accumulate, std::partial_sum
#include <chrono>   // std::this_thread::sleep_for
#include <thread>   // std::this_thread::sleep_for

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#define LABEL_WIDTH 20

// modules
#include "types.hpp"
#include <wrapper.h>
#include "convex_hull.hpp"
#include "log.hpp"

auto state2id(const std::vector<std::size_t> &state, const std::vector<std::size_t> &ex_pfx_product) {

    std::size_t id = 0;
    for (std::size_t dimension = 0; dimension < state.size(); ++dimension) {
        id += state[dimension] * ex_pfx_product[dimension];
    }
    return id;
}

auto id2state(const std::size_t id, const std::vector<std::size_t> &ex_pfx_product, const std::vector<std::size_t> &state_space_size) {

    std::vector<std::size_t> state(state_space_size.size());
    for (std::size_t dimension = 0; dimension < state_space_size.size(); ++dimension) {
        state[dimension] = (id / ex_pfx_product[dimension]) % state_space_size[dimension];
    }
    return state;
}

auto Q(PyObject *env, std::size_t action_space_size, const std::vector<std::size_t> &state, std::vector<std::vector<point>> &V, const double discount_factor) {

    std::vector<point> hull;
    for (std::size_t action = 0; action < action_space_size; ++action) {
        auto [ next_state, rewards ] = execute_action(env, state, action);
    }
    return hull;
}

void run_chvi(PyObject *env, const double discount_factor, const std::size_t max_iterations, const double epsilon) {

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

    auto start = std::chrono::system_clock::now();
    const auto state_space_size = get_observation_space_size(env);
    const auto n_states = std::accumulate(std::begin(state_space_size), std::end(state_space_size), 1ULL, std::multiplies<>());
    const auto action_space_size = get_action_space_size(env);

    log_line();
    log_title("Convex Hull Value Iteration");
    log_line();
    log_title("Environment Statistics");
    log_line();
    log_string("State space size", fmt::format("{} ({} states)", state_space_size, n_states));
    log_fmt("Action space size", action_space_size);
    log_line();
    log_title("Algorithm Parameters");
    log_line();
    log_fmt("Discount factor", discount_factor);
    log_fmt("Maximum number of iterations", max_iterations);
    log_fmt("Epsilon", epsilon);
    log_line();

    // data structure useful to associate each thread to a vector state
    std::vector<std::size_t> ex_pfx_product(state_space_size.size(), 1ULL);
    std::partial_sum(std::begin(state_space_size), std::end(state_space_size) - 1, std::begin(ex_pfx_product) + 1, std::multiplies<>());

    // output of the algorithm, a vector of convex hulls, one for each state
    std::vector<std::vector<point>> V(n_states);

    log_title("Iterations");
    log_line();
    double previous_delta = 0;
    std::size_t iteration = 0;

    while (++iteration <= max_iterations) {
        log_string(fmt::format("Iteration {}", iteration), "...");
        double delta = previous_delta + 1;
        if ((delta - previous_delta) / n_states < epsilon) {
            break;
        }
        previous_delta = delta;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    log_line();
    log_title("Algorithm Statistics");
    log_line();
    log_fmt("Executed iterations", std::min(iteration, max_iterations));
    log_string("Runtime", fmt::format("{:%T}", std::chrono::system_clock::now() - start));
    log_line();
}
