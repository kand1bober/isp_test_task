#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <string>
#include <algorithm>
#include <memory>

#include "bench_tasks.hpp"
    
class node_dispatcher_test {
private:
    node_dispatcher_test_context ctx_;
    std::vector<std::unique_ptr<task_t>> tasks_;
public:
    node_dispatcher_test(int32_t n, int32_t k, int32_t l, int32_t m) : 
        ctx_(n, k, l, m) 
    {
        tasks_.reserve(9);
        tasks_.push_back(create_nodes(ctx_));
        tasks_.push_back(fill_initial_resources(ctx_));
        tasks_.push_back(add_random_connections(ctx_));
        tasks_.push_back(measure_resources(ctx_));
        tasks_.push_back(add_random_resources(ctx_));
        tasks_.push_back(measure_resources(ctx_));
        tasks_.push_back(remove_random_edges(ctx_));
        tasks_.push_back(add_random_resources(ctx_));
        tasks_.push_back(measure_resources(ctx_));
    }

    void run() {
        Timer timer{};
        timer.start();

        for (const auto& task : tasks_) {
            task->run();
        }

        std::cout << "TOTAL: "; timer.print_elapsed();
    }
};
