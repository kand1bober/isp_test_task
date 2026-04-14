#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdint>

#include "pool.hpp"

class node_dispatcher_test {
private:
    using clock_t = std::chrono::high_resolution_clock;

    node_dispatcher graph_;
    std::mt19937 rng_{123456};

    int32_t N_ = 0;
    int32_t K_ = 0;
    int32_t L_ = 0;
    int32_t M_ = 0;

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
                graph_.add_connection(a, b);
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

    static void print_stage_time(
        const char* stage_name,
        const clock_t::time_point& start,
        const clock_t::time_point& end
    ) {
        std::cout << stage_name << ": "
                  << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
                  << " sec\n";
    }

public:
    node_dispatcher_test(int32_t n, int32_t k, int32_t l, int32_t m)
        : N_(n), K_(k), L_(l), M_(m) {}

    void run() {
        using std::cout;

        const auto t0 = clock_t::now();

        create_nodes();
        const auto t1 = clock_t::now();
        print_stage_time("Create nodes", t0, t1);

        fill_initial_resources();
        const auto t2 = clock_t::now();
        print_stage_time("Initial fill", t1, t2);

        add_random_connections();
        const auto t3 = clock_t::now();
        print_stage_time("Add connections", t2, t3);

        measure_resources();
        const auto t4 = clock_t::now();
        print_stage_time("First balancing", t3, t4);

        add_random_resources();
        const auto t5 = clock_t::now();
        print_stage_time("Add water", t4, t5);

        measure_resources();
        const auto t6 = clock_t::now();
        print_stage_time("Second balancing", t5, t6);

        remove_random_edges();
        const auto t7 = clock_t::now();
        print_stage_time("Remove edges", t6, t7);

        add_random_resources();
        const auto t8 = clock_t::now();
        print_stage_time("Add water again", t7, t8);

        measure_resources();
        const auto t9 = clock_t::now();
        print_stage_time("Final balancing", t8, t9);

        cout << "TOTAL: "
             << std::chrono::duration_cast<std::chrono::seconds>(t9 - t0).count()
             << " sec\n";
    }
};

