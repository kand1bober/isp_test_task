#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <string>
#include <memory>

#include "pool.hpp"
#include "utilities.hpp"

class node_dispatcher_test_context {
public:
    node_dispatcher graph_;
    std::mt19937 rng_{123456};

    int32_t N_, K_, L_, M_;
    std::vector<std::pair<int32_t, int32_t>> edges_;

    node_dispatcher_test_context(int32_t n, int32_t k, int32_t l, int32_t m) 
        : N_(n), K_(k), L_(l), M_(m) {};
};


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


//------------- Declarations -------------
std::unique_ptr<task_t> create_nodes(node_dispatcher_test_context& ctx_);

std::unique_ptr<task_t> fill_initial_resources(node_dispatcher_test_context& ctx_);

std::unique_ptr<task_t> add_random_connections(node_dispatcher_test_context& ctx_);

std::unique_ptr<task_t> measure_resources(node_dispatcher_test_context& ctx_);

std::unique_ptr<task_t> add_random_resources(node_dispatcher_test_context& ctx_);

std::unique_ptr<task_t> remove_random_edges(node_dispatcher_test_context& ctx_);
