#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <string>
#include <algorithm>
#include <memory>

#include "pool.hpp"
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
        tasks_.push_back(std::make_unique<create_nodes_t>(ctx_));
        tasks_.push_back(std::make_unique<fill_initial_resources_t>(ctx_));
        tasks_.push_back(std::make_unique<add_random_connections_t>(ctx_));
        tasks_.push_back(std::make_unique<measure_resources_t>(ctx_));
        tasks_.push_back(std::make_unique<add_random_resources_t>(ctx_));
        tasks_.push_back(std::make_unique<measure_resources_t>(ctx_));
        tasks_.push_back(std::make_unique<remove_random_edges_t>(ctx_));
        tasks_.push_back(std::make_unique<add_random_resources_t>(ctx_));
        tasks_.push_back(std::make_unique<measure_resources_t>(ctx_));
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
