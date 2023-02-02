#include "main.hpp"

#include <iostream>     // std::cout, std::cerr
#include <algorithm>    // std::sort
#include <sys/time.h>   // timer stuff
#include <unistd.h>     // getopt
#include <chrono>       // std::this_thread::sleep_for
#include <thread>       // std::this_thread::sleep_for
#include <set>          // std::set
#include <string.h>     // strdup

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
                    std::cerr << argv[0] << ": file not found -- '";
                    std::cerr << optarg << "'" << '\n';
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
        std::cerr << argv[0] << ": instance not specified!\n";
        return EXIT_FAILURE;
    }

    log_line();
    log_value("Instance", instance, "i");
    log_value("Seed", seed, "s");
    log_line();

    //log_line();

    return EXIT_SUCCESS;
}
