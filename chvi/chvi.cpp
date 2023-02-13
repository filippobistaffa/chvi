#include "chvi.hpp"

#include <numeric>  // std::accumulate, std::partial_sum
#include <chrono>   // std::this_thread::sleep_for
#include <thread>   // std::this_thread::sleep_for
#include <set>      // std::set

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>

// modules
#include "types.hpp"
#include <wrapper.h>
#include "convex_hull.hpp"
#include "log.hpp"

// remove dominated points from convex hull
constexpr bool PARTIAL = false;

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

auto Q(PyObject *env, const std::vector<std::size_t> &state_space_size, std::size_t action_space_size, const std::size_t id,
       const std::vector<std::size_t> &ex_pfx_product, const std::vector<std::vector<point>> &hulls, const double discount_factor) {

    std::set<point> points;
    const auto state = id2state(id, ex_pfx_product, state_space_size);

    for (std::size_t action = 0; action < action_space_size; ++action) {
        const auto [ next_state, rewards ] = execute_action(env, state, action);
        //fmt::print("Executed action {} on state {} (id: {}) -> new state: {} rewards: {}\n", action, state, id, next_state, rewards);
        const auto next_state_hull = hulls[state2id(next_state, ex_pfx_product)];
        //fmt::print("hull {}\n", next_state_hull);
        const auto transformed_hull = linear_transformation(next_state_hull, discount_factor, rewards);
        //fmt::print("transformed {}\n", transformed_hull);
        points.insert(std::begin(transformed_hull), std::end(transformed_hull));
    }

    if (PARTIAL) {
        return non_dominated(convex_hull(points));
    } else {
        return convex_hull(points);
    }
}

std::vector<std::vector<point>> run_chvi(PyObject *env, const double discount_factor, const std::size_t max_iterations,
                                         const double epsilon, const bool verbose) {

    auto start = std::chrono::system_clock::now();
    const auto state_space_size = get_observation_space_size(env);
    const auto n_states = std::accumulate(std::begin(state_space_size), std::end(state_space_size), 1ULL, std::multiplies<>());
    const auto action_space_size = get_action_space_size(env);

    if (verbose) {
        log_line();
        log_title("Convex Hull Value Iteration");
        log_title("https://github.com/filippobistaffa/chvi");
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
    }

    // data structure useful to associate each thread to a vector state
    std::vector<std::size_t> ex_pfx_product(state_space_size.size(), 1ULL);
    std::partial_sum(std::begin(state_space_size), std::end(state_space_size) - 1, std::begin(ex_pfx_product) + 1, std::multiplies<>());

    // output of the algorithm, a vector of convex hulls, one for each state
    std::vector<std::vector<point>> hulls(n_states);
    //fmt::print("Initial hulls: {}\n", hulls);

    if (verbose) {
        log_title("Iterations");
        log_line();
    }

    std::size_t iteration = 0;
    double previous_delta = 0;

    while (++iteration <= max_iterations) {
        double delta = 0;
        std::vector<std::vector<point>> new_hulls(n_states);
        for (std::size_t id = 0; id < n_states; ++id) {
            if (!is_terminal(env, id2state(id, ex_pfx_product, state_space_size))) {
                //fmt::print("ID: {} -> {}\n", id, id2state(id, ex_pfx_product, state_space_size));
                new_hulls[id] = Q(env, state_space_size, action_space_size, id, ex_pfx_product, hulls, discount_factor);
                //fmt::print("Hull: {}\n", hull);
                delta += new_hulls[id].size();
            }
        }
        hulls = new_hulls;
        if (verbose) {
            log_string(fmt::format("Iteration {}", iteration), fmt::format("Relative difference: {:.3f}", std::abs(delta - previous_delta) / n_states));
        }
        if (std::abs(delta - previous_delta) / n_states <= epsilon) {
            break;
        }
        previous_delta = delta;
    }

    if (verbose) {
        log_line();
        log_title("Algorithm Statistics");
        log_line();
        log_fmt("Executed iterations", std::min(iteration, max_iterations));
        log_string("Runtime", fmt::format("{:%T}", std::chrono::system_clock::now() - start));
        log_line();
    }

    return hulls;
}
