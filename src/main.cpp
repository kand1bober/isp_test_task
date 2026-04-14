#include <iostream>
#include <string>
#include <cstdint>

#include "../include/bench.hpp"
#include "../include/unit_tests.hpp"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        const std::string mode = argv[1];

        if (mode == "-test") {
            node_dispatcher_unit_tests tests;
            const bool ok = tests.run_all();
            return ok ? 0 : 1;
        }

        if (mode == "-bench") {
            const int32_t N = 38'000'000;
            const int32_t K = 13'000'000;
            const int32_t L = 31'000'000;
            const int32_t M = 4'000'000;

            node_dispatcher_test benchmark(N, K, L, M);
            benchmark.run();
            return 0;
        }
    }

    std::cout << "Usage:\n";
    std::cout << "    ./prog -test   -> run unit tests\n";
    std::cout << "    ./prog -bench  -> run benchmark scenario\n";

    return 0;
}
