#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdint>
#include <string>
#include <algorithm>

#include "pool.hpp"

class time_work {
private:
    using clock_t = std::chrono::high_resolution_clock;
    clock_t::time_point clock_;

public:
    time_work() = default;

    void start_clock() {
        clock_ = clock_t::now();
    }

    auto get_clock_time() const {
        return clock_t::now() - clock_;
    }

    void print_clock_time() const {
        std::cout
            << std::chrono::duration_cast<std::chrono::seconds>(get_clock_time()).count()
            << " sec\n";
    }
};

class node_dispatcher_test {
private: 
    time_work stage_timer_;
    time_work total_timer_;

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
    
    void start_stage_time_measure(const std::string& stage_name) {
        std::cout << stage_name << ": ";
        stage_timer_.start_clock();
    }

    void end_stage_time_measure() {
        stage_timer_.print_clock_time();
    }

public:
    node_dispatcher_test(int32_t n, int32_t k, int32_t l, int32_t m)
        : N_(n), K_(k), L_(l), M_(m) {}

    void run() {
        total_timer_.start_clock();

        start_stage_time_measure("Create nodes");
        create_nodes();
        end_stage_time_measure();

        start_stage_time_measure("Initial fill");
        fill_initial_resources();
        end_stage_time_measure();

        start_stage_time_measure("Add connections");
        add_random_connections();
        end_stage_time_measure();

        start_stage_time_measure("First balancing");
        measure_resources();
        end_stage_time_measure();

        start_stage_time_measure("Add water");
        add_random_resources();
        end_stage_time_measure();

        start_stage_time_measure("Second balancing");
        measure_resources();
        end_stage_time_measure();

        start_stage_time_measure("Remove edges");
        remove_random_edges();
        end_stage_time_measure();

        start_stage_time_measure("Add water again");
        add_random_resources();
        end_stage_time_measure();

        start_stage_time_measure("Final balancing");
        measure_resources();
        end_stage_time_measure();

        std::cout << "TOTAL: ";
        total_timer_.print_clock_time();
    }

};
