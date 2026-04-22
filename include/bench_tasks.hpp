#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <string>

#include "pool.hpp"
#include "utilities.hpp"

class task_t {
private: 
    Timer clock;
    std::string task_name_;

protected:
    virtual void task() = 0; 

public: 
    task_t(std::string name) : task_name_(name){};
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


class create_nodes_t final : public task_t {
private:
    node_dispatcher_test_context& ctx_;

    void task() override {
        for (int32_t i = 0; i < ctx_.N_; ++i)
            ctx_.graph_.add_node();
    }

public:
    create_nodes_t(node_dispatcher_test_context& ctx) : 
        task_t("create_nodes"), ctx_(ctx) {}
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
    fill_initial_resources_t(node_dispatcher_test_context& ctx) : 
        task_t("fill_initial_resources"), ctx_(ctx) {}
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
    add_random_connections_t(node_dispatcher_test_context& ctx) : 
        task_t("add_random_connections"), ctx_(ctx) {}            
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
    add_random_resources_t(node_dispatcher_test_context& ctx) : 
        task_t("add_random_resources"), ctx_(ctx) {}
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
    remove_random_edges_t(node_dispatcher_test_context& ctx) : 
        task_t("remove_random_edges"), ctx_(ctx) {}
};


class measure_resources_t final : public task_t {
private:
    node_dispatcher_test_context& ctx_;
    
    void task() override {
        ctx_.graph_.calculate_resources();
    }

public:
    measure_resources_t(node_dispatcher_test_context& ctx) : 
        task_t("measure_resources"), ctx_(ctx) {}
};
