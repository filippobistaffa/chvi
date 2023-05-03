#include "chvi.hpp"

#include <numeric>  // std::accumulate, std::partial_sum
#include <chrono>   // std::this_thread::sleep_for
#include <thread>   // std::this_thread::sleep_for
#include <atomic>   // std::atomic
#include <set>      // std::set

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>

// modules
#include "types.hpp"
#include "convex_hull.hpp"
#include "log.hpp"

#ifdef CYTHON
#include <wrapper.h>
#else
#include <omp.h>    // omp_get_max_threads
#endif

#ifdef CPU_PROFILER
#define CPU_PROFILER_OUTPUT "trace.prof"
#include <gperftools/profiler.h>
#endif

#ifdef HEAP_PROFILER
#define HEAP_PROFILER_PREFIX "memory/dump"
#include <gperftools/heap-profiler.h>
#endif

// remove dominated points from convex hull
constexpr bool PARTIAL = true;

std::atomic<std::size_t> recomputed = 0;
std::atomic<std::size_t> non_recomputed = 0;

auto state2id(const std::vector<coordinate> &state, const std::vector<std::size_t> &ex_pfx_product) {

    std::size_t id = 0;

    for (std::size_t dimension = 0; dimension < state.size(); ++dimension) {
        id += state[dimension] * ex_pfx_product[dimension];
    }

    return id;
}

auto id2state(const std::size_t id, const std::vector<std::size_t> &ex_pfx_product, const std::vector<std::size_t> &state_space_size) {

    std::vector<coordinate> state(state_space_size.size());

    for (std::size_t dimension = 0; dimension < state_space_size.size(); ++dimension) {
        state[dimension] = (id / ex_pfx_product[dimension]) % state_space_size[dimension];
    }

    return state;
}

auto Q(env_type env, const std::vector<std::size_t> &state_space_size, std::size_t action_space_size, const std::size_t id,
       const std::vector<std::size_t> &ex_pfx_product, const std::vector<std::vector<coordinate>> &hulls,
       std::vector<std::vector<coordinate>> &old_non_dominated, const double discount_factor) {

    std::set<std::vector<coordinate>> unique;
    const auto state = id2state(id, ex_pfx_product, state_space_size);

    for (std::size_t action = 0; action < action_space_size; ++action) {
        const auto [ next_state, rewards ] = execute_action(env, state, action);
        //fmt::print("Executed action {} on state {} (id: {}) -> new state: {} rewards: {}\n", action, state, id, next_state, rewards);
        const auto next_state_hull = hulls[state2id(next_state, ex_pfx_product)];
        //fmt::print("hull {}\n", next_state_hull);
        const auto transformed_hull = linear_transformation(next_state_hull, discount_factor, rewards);
        //fmt::print("transformed {}\n", transformed_hull);
        for (std::size_t i = 0; i < transformed_hull.size(); i += state.size()) {
            unique.insert(std::vector<coordinate>(std::begin(transformed_hull) + i, std::begin(transformed_hull) + i + state.size()));
        }
    }

    if (PARTIAL) {
        const auto new_non_dominated = non_dominated(std::vector(std::begin(unique), std::end(unique)));
        const auto flat_new_non_dominated = flatten(new_non_dominated);
        if (flat_new_non_dominated == old_non_dominated[id]) {
            non_recomputed++;
            return hulls[id];
        } else {
            recomputed++;
            old_non_dominated[id] = std::move(flat_new_non_dominated);
            return flatten(non_dominated(convex_hull(new_non_dominated)));
        }
    } else {
        return flatten(convex_hull(unique));
    }
}

std::vector<std::vector<coordinate>> run_chvi(env_type env, const double discount_factor, const std::size_t max_iterations,
                                              const double epsilon, const bool verbose) {

    auto start = std::chrono::system_clock::now();
    const auto state_space_size = get_observation_space_size(env);
    const auto n_states = std::accumulate(std::begin(state_space_size), std::end(state_space_size), 1ULL, std::multiplies<>());
    const auto n_goals = get_n_goals(env);
    const auto action_space_size = get_action_space_size(env);

    if (verbose) {
        log_line();
        log_title("Convex Hull Value Iteration");
        log_title("https://github.com/filippobistaffa/chvi");
        log_line();
        log_title("Environment Statistics");
        log_line();
        log_string("State space size", fmt::format("{} ({} states)", state_space_size, n_states));
        log_string("Number of goal states", fmt::format("{} ({:.2f}%)", n_goals, 100.0 * n_goals / n_states));
        log_fmt("Action space size", action_space_size);
        log_line();
        log_title("Algorithm Parameters");
        log_line();
        log_fmt("Discount factor", discount_factor);
        log_fmt("Maximum number of iterations", max_iterations);
        log_fmt("Epsilon", epsilon);
        log_string("Precision", fmt::format("{} bits", sizeof(coordinate) * 8));
        #ifndef CYTHON
        log_fmt("Available parallel threads", omp_get_max_threads());
        #endif
        log_line();
    }

    // data structure useful to associate each thread to a vector state
    std::vector<std::size_t> ex_pfx_product(state_space_size.size(), 1ULL);
    std::partial_sum(std::begin(state_space_size), std::end(state_space_size) - 1, std::begin(ex_pfx_product) + 1, std::multiplies<>());

    // output of the algorithm, a vector of convex hulls (flat vector of coordinates), one for each state
    std::vector<std::vector<coordinate>> hulls(n_states);
    //fmt::print("Initial hulls: {}\n", hulls);
    std::vector<std::vector<coordinate>> old_non_dominated(n_states);

    if (verbose) {
        log_title("Relative Difference");
        log_line();
    }

    std::size_t iteration = 0;
    double previous_delta = 0;

    #ifdef CPU_PROFILER
    ProfilerStart(CPU_PROFILER_OUTPUT);
    #endif

    #ifdef HEAP_PROFILER
    HeapProfilerStart(HEAP_PROFILER_PREFIX);
    #endif

    while (++iteration <= max_iterations) {
        double delta = 0;
        std::vector<std::vector<coordinate>> new_hulls(n_states);
        #ifndef CYTHON
        #pragma omp parallel for reduction(+:delta)
        #endif
        for (std::size_t id = 0; id < n_states; ++id) {
            if (!is_terminal(env, id2state(id, ex_pfx_product, state_space_size))) {
                //fmt::print("ID: {} -> {}\n", id, id2state(id, ex_pfx_product, state_space_size));
                new_hulls[id] = Q(env, state_space_size, action_space_size, id, ex_pfx_product, hulls, old_non_dominated, discount_factor);
                //fmt::print("Hull: {}\n", new_hulls[id]);
                delta += new_hulls[id].size() / state_space_size.size();
            }
        }
        hulls = std::move(new_hulls);
        if (verbose) {
            log_string(fmt::format("Iteration {}", iteration), fmt::format("{:.5f} ({})", std::abs(delta - previous_delta) / n_states, delta));
        }
        if (std::abs(delta - previous_delta) / n_states <= epsilon) {
            break;
        }
        previous_delta = delta;
        #ifdef HEAP_PROFILER
        auto memory_a = sizeof(hulls);
        auto memory_b = 0ULL;
        for (auto hull : hulls) {
            memory_a += sizeof(hull);
            memory_a += hull.size() * sizeof(coordinate);
            memory_b += hull.size() * sizeof(coordinate);
        }
        #define MB(X) ((1.0f * (X)) / (1024 * 1024))
        fmt::print("Memory (total, points): {:.1f} MB, {:.1f} MB\n", MB(memory_a), MB(memory_b));
        HeapProfilerDump(fmt::format("Iteration {}", iteration).c_str());
        #endif
    }

    #ifdef CPU_PROFILER
    ProfilerStop();
    #endif

    #ifdef HEAP_PROFILER
    HeapProfilerStop();
    #endif

    if (verbose) {
        log_line();
        log_title("Algorithm Statistics");
        log_line();
        log_fmt("Executed iterations", std::min(iteration, max_iterations));
        log_fmt("Avoided convex hull recomputations", fmt::format("{}/{} ({:.2f}%)",
            non_recomputed, recomputed + non_recomputed, 100.0 * non_recomputed / (recomputed + non_recomputed))
        );
        log_string("Runtime", fmt::format("{:%T}", std::chrono::system_clock::now() - start));
        log_line();
    }

    return hulls;
}
