#include "main.hpp"

#include <sys/time.h>   // timer stuff
#include <unistd.h>     // getopt
#include <chrono>       // std::this_thread::sleep_for
#include <thread>       // std::this_thread::sleep_for
#include <set>          // std::set

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/format.h>

// Modules
#include "log.hpp"
#include "ch.hpp"

#define DELTAT(T2, T1) ((float)((T2).tv_usec - (T1).tv_usec) / 1e6 + (T2).tv_sec - (T1).tv_sec) // Seconds
float prev;

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

    point p0 {0, 0, 4};
    point p1 {0, 5, 3};
    point p2 {1, 7, 0};
    point p3 {2, 1, 4};
    point p4 {3, 4, 5};
    point p5 {4, 2, 3};
    point p6 {4, 4, 6};
    point p7 {4, 6, 7};
    point p8 {5, 0, 2};
    point p9 {6, 4, 1};
    point p10 {6, 5, 1};
    point p11 {6, 7, 0};
    point p12 {7, 4, 3};

    std::set<point> points;
    points.insert(p0);
    points.insert(p1);
    points.insert(p2);
    points.insert(p3);
    points.insert(p4);
    points.insert(p5);
    points.insert(p6);
    points.insert(p7);
    points.insert(p8);
    points.insert(p9);
    points.insert(p10);
    points.insert(p11);
    points.insert(p12);

    convex_hull(points);

    //log_line();

    return EXIT_SUCCESS;
}
