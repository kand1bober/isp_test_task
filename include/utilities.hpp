#pragma once

#include <chrono>
#include <iostream>

class Timer {
private:
    using clock_t = std::chrono::high_resolution_clock;
    clock_t::time_point clock_;

public:
    Timer() = default;

    void start() {
        clock_ = clock_t::now();
    }

    auto elapsed() const {
        return clock_t::now() - clock_;
    }

    void print_elapsed() const {
        std::cout
            << std::chrono::duration_cast<std::chrono::seconds>(elapsed()).count()
            << " sec\n";
    }
};
