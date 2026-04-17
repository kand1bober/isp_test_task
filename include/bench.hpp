#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdint>
#include <string>
#include <algorithm>

#include "pool.hpp"

class node_dispatcher_test {
private:
    node_dispatcher graph_;
    std::mt19937 rng_{123456};

    int32_t N_, K_, L_, M_;
    std::vector<std::pair<int32_t, int32_t>> edges_;

    void create_nodes() {
        for (int32_t i = 0; i < N_; ++i) {
            graph_.add_node();
        }
    }

    void fill_initial_resources() {
        std::uniform_int_distribution<int32_t> water_dist(1, 500);

        for (int32_t i = 0; i < N_; ++i) {
            graph_.add_resource(i, water_dist(rng_));
        }
    }

    void add_random_connections() {
        std::uniform_int_distribution<int32_t> node_dist(0, N_ - 1);

        edges_.reserve(K_);

        for (int32_t i = 0; i < K_; ++i) {
            int32_t a = node_dist(rng_);
            int32_t b = node_dist(rng_);

            if (a != b) {
                graph_.add_edge(a, b);
                edges_.emplace_back(a, b);
            }
        }
    }

    void add_random_resources() {
        std::uniform_int_distribution<int32_t> node_dist(0, N_ - 1);
        std::uniform_int_distribution<int32_t> water_dist(1, 500);

        for (int32_t i = 0; i < L_; ++i) {
            graph_.add_resource(node_dist(rng_), water_dist(rng_));
        }
    }

    void remove_random_edges() {
        const size_t remove_count = std::min(static_cast<size_t>(M_), edges_.size());

        for (size_t i = 0; i < remove_count; ++i) {
            const auto [a, b] = edges_[i];
            graph_.remove_edge(a, b);
        }
    }

    void measure_resources() {
        graph_.calculate_resources();
    }

public:
    node_dispatcher_test(int32_t n, int32_t k, int32_t l, int32_t m)
        : N_(n), K_(k), L_(l), M_(m) {}

    void run();
};


class time_work {
private:
    using clock_t = std::chrono::high_resolution_clock;
    clock_t::time_point clock_;

public:
    time_work() = default;

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


class task_t {
private: 
    time_work clock;

public: 
    task_t() = default;

    void run(
        const std::string& work_name,
        node_dispatcher_test* obj, 
        void (node_dispatcher_test::*method_ptr)()
    ) {
        std::cout << work_name << ": ";     
        clock.start();
        (obj->*method_ptr)();
        clock.print_elapsed();
    }  
};


void node_dispatcher_test::run() {
    time_work clock{};
    clock.start();

    task_t task{};
    task.run("create_nodes", this, &node_dispatcher_test::create_nodes);    
    task.run("fill_initial_resources", this, &node_dispatcher_test::fill_initial_resources);
    task.run("add_random_connections", this, &node_dispatcher_test::add_random_connections);
    task.run("measure_resources (1)", this, &node_dispatcher_test::measure_resources);
    task.run("add_random_resources (1)", this, &node_dispatcher_test::add_random_resources);
    task.run("measure_resources (2)", this, &node_dispatcher_test::measure_resources);
    task.run("remove_random_edges", this, &node_dispatcher_test::remove_random_edges);
    task.run("add_random_resources (2)", this, &node_dispatcher_test::add_random_resources);
    task.run("measure_resources (3)", this, &node_dispatcher_test::measure_resources);

    std::cout << "TOTAL: "; clock.print_elapsed();
}
