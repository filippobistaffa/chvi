#ifndef LOG_HPP_
#define LOG_HPP_

#include <iostream>     // std::cout
#include <iomanip>      // std::setw

#define TOTAL_WIDTH 79
#define COLUMN_WIDTH ((TOTAL_WIDTH - 7) / 2)

static float progress;

__attribute__((always_inline)) inline
void log_line() {

    std::cout << "+";
    for (size_t i = 0; i < COLUMN_WIDTH + 2; ++i) {
        std::cout << "-";
    }
    std::cout << "+";
    for (size_t i = 0; i < COLUMN_WIDTH + 2; ++i) {
        std::cout << "-";
    }
    std::cout << "+" << '\n';
}

template <typename T>
__attribute__((always_inline)) inline
void log_value(std::string name, T val, std::string param = "") {

    std::cout << "| ";
    const size_t par_space = param.length() + param.length() ? 5 : 0;
    if (name.length() > COLUMN_WIDTH - par_space) {
        std::cout << name.substr(0, COLUMN_WIDTH - 3 - par_space) << "...";
    } else {
        std::cout << std::setw(COLUMN_WIDTH - par_space) << std::left << name;
    }
    if (param.length()) {
        std::cout << " (-" << param << ")";
    }
    std::cout << " | ";
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
    std::cout << " |" << '\n';
}

__attribute__((always_inline)) inline
void log_progress_increase(float step, float tot) {

    if (progress == tot) {
        return;
    }
    if (progress == 0) {
        std::cout << "|";
    }
    const size_t cur_p = (TOTAL_WIDTH - 2) * (progress / tot);
    const size_t new_p = (TOTAL_WIDTH - 2) * ((progress + step) / tot);
    for (size_t i = cur_p; i < new_p; ++i) {
        std::cout << "·" << std::flush;;
    }
    progress += step;
    if (progress == tot) {
        std::cout << "|" << '\n';
    }
}

#endif /* LOG_HPP_ */
