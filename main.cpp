#include "main.hpp"

#include <sys/time.h>   // timer stuff
#include <unistd.h>     // getopt
#include <set>          // std::set

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ranges.h>

// Modules
#include "convex_hull.hpp"
#include "log.hpp"

#define DELTAT(T2, T1) ((float)((T2).tv_usec - (T1).tv_usec) / 1e6 + (T2).tv_sec - (T1).tv_sec) // Seconds
float prev;

#define LABEL_WIDTH 20

template <typename type>
inline auto show_stat(struct timeval t1, type stat) {

    struct timeval t2;
    gettimeofday(&t2, nullptr);
    float elapsed = DELTAT(t2, t1);
    char t[50];
    sprintf(t, "%23.5f (+%9.5f)", elapsed, elapsed - prev);
    log_value(t, stat);
    prev = elapsed;
}

inline bool exists(const char *filename) {

    FILE *file = fopen(filename, "r");
    if (!file) {
        return false;
    } else {
        fclose(file);
        return true;
    }
}

// Default parameters
int seed = 0;           // Seed

int main(int argc, char *argv[]) {

    char *instance = nullptr;
    int opt;

    while ((opt = getopt(argc, argv, "i:s:")) != -1) {
        switch (opt) {
            case 'i': // input instance
                if (exists(optarg)) {
                    instance = optarg;
                } else {
                    fmt::print(stderr, "{}: file not found -- '{}'\n", argv[0], optarg);
                    return EXIT_FAILURE;
                }
                continue;
            case 's': // seed
                seed = atoi(optarg);
                continue;
            default:
                return EXIT_FAILURE;
        }
    }

    if (!instance) {
        fmt::print(stderr, "{}: instance not specified!\n", argv[0]);
        return EXIT_FAILURE;
    }

    log_line();
    log_value("Instance", instance, "i");
    log_value("Seed", seed, "s");
    log_line();

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

    //log_line();

    return EXIT_SUCCESS;
}
