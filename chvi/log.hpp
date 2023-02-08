#ifndef LOG_HPP_
#define LOG_HPP_

#include <iostream>     // std::cout
#include <iomanip>      // std::setw

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>

#define TOTAL_WIDTH 79
#define COLUMN_WIDTH ((TOTAL_WIDTH - 7) / 2)

static float progress;

void log_title(std::string title) {

    fmt::print("+ ");
    fmt::print("{1:^{0}}", TOTAL_WIDTH - 4, title);
    fmt::print(" +\n");
}

void log_line() {

    fmt::print("+");
    fmt::print("{:->{}}", "", COLUMN_WIDTH + 2);
    fmt::print("+");
    fmt::print("{:->{}}", "", COLUMN_WIDTH + 2);
    fmt::print("+\n");
}

template <typename T>
__attribute__((always_inline)) inline
void log_value(std::string name, T val, std::string param = "") {

    fmt::print("| ");
    const size_t par_space = param.length() + param.length() ? 5 : 0;
    if (name.length() > COLUMN_WIDTH - par_space) {
        std::cout << name.substr(0, COLUMN_WIDTH - 3 - par_space) << "...";
    } else {
        std::cout << std::setw(COLUMN_WIDTH - par_space) << std::left << name;
    }
    if (param.length()) {
        std::cout << " (-" << param << ")";
    }
    fmt::print(" | ");
    if constexpr (std::is_same<T, char *>::value) {
        const auto str = std::string(val);
        if (str.length() > COLUMN_WIDTH) {
            std::cout << "..." << str.substr(str.length() - COLUMN_WIDTH + 3);
        } else {
            std::cout << std::setw(COLUMN_WIDTH) << std::left << val;
        }
    } else {
        std::cout << std::setw(COLUMN_WIDTH) << std::left << val;
    }
    fmt::print(" |\n");
}

__attribute__((always_inline)) inline
void log_progress_increase(float step, float tot) {

    if (progress == tot) {
        return;
    }
    if (progress == 0) {
        fmt::print("|");
    }
    const size_t cur_p = (TOTAL_WIDTH - 2) * (progress / tot);
    const size_t new_p = (TOTAL_WIDTH - 2) * ((progress + step) / tot);
    for (size_t i = cur_p; i < new_p; ++i) {
        fmt::print(".");
        std::fflush(nullptr);
    }
    progress += step;
    if (progress == tot) {
        fmt::print("|\n");
    }
}

#endif /* LOG_HPP_ */
