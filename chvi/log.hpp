#ifndef LOG_HPP_
#define LOG_HPP_

// fmt library
#define FMT_HEADER_ONLY
#include <fmt/core.h>

#define TOTAL_WIDTH 79
#define COLUMN_WIDTH ((TOTAL_WIDTH - 7) / 2)

#define log_fmt(A, B) log_string(A, fmt::format("{}", B))

static float progress;

void log_title(std::string title) {

    fmt::print("| {1:^{0}} |\n", TOTAL_WIDTH - 4, title);
    std::fflush(nullptr);
}

void log_line() {

    fmt::print("+{1:->{0}}+{1:->{0}}+\n", COLUMN_WIDTH + 2, "");
    std::fflush(nullptr);
}

void log_string(std::string name, std::string val, std::string param = "") {

    fmt::print("| ");
    const size_t par_space = param.length() + param.length() ? 5 : 0;
    if (name.length() > COLUMN_WIDTH - par_space) {
        fmt::print("{}...", name.substr(0, COLUMN_WIDTH - 3 - par_space));
    } else {
        fmt::print("{1:<{0}}", COLUMN_WIDTH - par_space, name);
    }
    if (param.length()) {
        fmt::print(" (-{})", param);
    }
    fmt::print(" | ");
    if (val.length() > COLUMN_WIDTH) {
        fmt::print("...{}", val.substr(val.length() - COLUMN_WIDTH + 3));
    } else {
        fmt::print("{1:<{0}}", COLUMN_WIDTH, val);
    }
    fmt::print(" |\n");
    std::fflush(nullptr);
}

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
        fmt::print("·");
        std::fflush(nullptr);
    }
    progress += step;
    if (progress == tot) {
        fmt::print("|\n");
    }
}

#endif /* LOG_HPP_ */
