#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdint>
#include <string>
#include <algorithm>

#include "pool.hpp"


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


class task_t {
private: 
    Timer clock;

protected:
    std::string task_name_;
    virtual void task() = 0; 

public: 
    task_t() = default;
    virtual ~task_t() = default;

    void run() {
        std::cout << task_name_ << ": ";
        clock.start();
        task();
        clock.print_elapsed();
    }
};


class node_dispatcher_test_context {
public:
    node_dispatcher graph_;
    std::mt19937 rng_{123456};

    int32_t N_, K_, L_, M_;
    std::vector<std::pair<int32_t, int32_t>> edges_;

    node_dispatcher_test_context(int32_t n, int32_t k, int32_t l, int32_t m) 
        : N_(n), K_(k), L_(l), M_(m) {};
};


class node_dispatcher_test {

    class create_nodes_t final : public task_t {
        private:
            node_dispatcher_test_context& ctx_;

            void task() override {
                for (int32_t i = 0; i < ctx_.N_; ++i)
                    ctx_.graph_.add_node();
            }

        public:
            create_nodes_t(node_dispatcher_test_context& ctx) : ctx_{ctx} {
                task_t::task_name_ = "create_nodes";
            }
        };

    class fill_initial_resources_t final : public task_t {
        private:
            node_dispatcher_test_context& ctx_;
            
            void task() override {
                std::uniform_int_distribution<int32_t> water_dist(1, 500);

                for (int32_t i = 0; i < ctx_.N_; ++i) {
                    ctx_.graph_.add_resource(i, water_dist(ctx_.rng_));
                }            
            }

        public:
            fill_initial_resources_t(node_dispatcher_test_context& ctx) : ctx_{ctx} {
                task_t::task_name_ = "fill_initial_resources";
            }
        };

    class add_random_connections_t final : public task_t {
        private:
            node_dispatcher_test_context& ctx_;
            
            void task() override {
                std::uniform_int_distribution<int32_t> node_dist(0, ctx_.N_ - 1);
                ctx_.edges_.reserve(ctx_.K_);

                for (int32_t i = 0; i < ctx_.K_; ++i) {
                    int32_t a = node_dist(ctx_.rng_);
                    int32_t b = node_dist(ctx_.rng_);

                    if (a != b) {
                        ctx_.graph_.add_edge(a, b);
                        ctx_.edges_.emplace_back(a, b);
                    }
                }
            }

        public:
            add_random_connections_t(node_dispatcher_test_context& ctx) : ctx_{ctx} {
                task_t::task_name_ = "add_random_connections";
            }            
        };
    
    class add_random_resources_t final : public task_t {
        private:
            node_dispatcher_test_context& ctx_;
            
            void task() override {
                std::uniform_int_distribution<int32_t> node_dist(0, ctx_.N_ - 1);
                std::uniform_int_distribution<int32_t> water_dist(1, 500);

                for (int32_t i = 0; i < ctx_.L_; ++i) {
                    ctx_.graph_.add_resource(node_dist(ctx_.rng_), water_dist(ctx_.rng_));
                }
            }

        public:
            add_random_resources_t(node_dispatcher_test_context& ctx) : ctx_{ctx} {
                task_t::task_name_ = "add_random_resources";
            }
        };


    class remove_random_edges_t final : public task_t {
        private:
            node_dispatcher_test_context& ctx_;
            
            void task() override {
                const size_t remove_count = std::min(static_cast<size_t>(ctx_.M_), ctx_.edges_.size());

                for (size_t i = 0; i < remove_count; ++i) {
                    const auto [a, b] = ctx_.edges_[i];
                    ctx_.graph_.remove_edge(a, b);
                }
            }

        public:
            remove_random_edges_t(node_dispatcher_test_context& ctx) : ctx_{ctx} {
                task_t::task_name_ = "remove_random_edges";
            }
        };


    class measure_resources_t final : public task_t {
        private:
            node_dispatcher_test_context& ctx_;
            
            void task() override {
                ctx_.graph_.calculate_resources();
            }

        public:
            measure_resources_t(node_dispatcher_test_context& ctx) : ctx_{ctx} {
                task_t::task_name_ = "measure_resources";
            }
        };

private:
    node_dispatcher_test_context ctx_;

    create_nodes_t create_nodes_;
    fill_initial_resources_t fill_initial_resources_;
    add_random_connections_t add_random_connections_;
    add_random_resources_t add_random_resources_;
    remove_random_edges_t remove_random_edges_;
    measure_resources_t measure_resources_;

public:
    node_dispatcher_test(int32_t n, int32_t k, int32_t l, int32_t m) : 
        ctx_(n, k, l, m),
        create_nodes_{ctx_},
        fill_initial_resources_{ctx_},
        add_random_connections_{ctx_},
        add_random_resources_{ctx_},
        remove_random_edges_{ctx_},
        measure_resources_{ctx_} 
    {}

    void run() {
        Timer timer{};
        timer.start();

        create_nodes_.run();
        fill_initial_resources_.run();
        add_random_connections_.run();
        measure_resources_.run();
        add_random_resources_.run();
        measure_resources_.run();
        remove_random_edges_.run();
        add_random_resources_.run();
        measure_resources_.run();

        std::cout << "TOTAL: "; timer.print_elapsed();
    }
};
