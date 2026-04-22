#include "../include/bench_tasks.hpp"

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


//------------- Functions -------------
std::unique_ptr<task_t> create_nodes(node_dispatcher_test_context& ctx_) {
    return std::make_unique<create_nodes_t>(ctx_);
}

std::unique_ptr<task_t> fill_initial_resources(node_dispatcher_test_context& ctx_) {
    return std::make_unique<fill_initial_resources_t>(ctx_);
}

std::unique_ptr<task_t> add_random_connections(node_dispatcher_test_context& ctx_) {
    return std::make_unique<add_random_connections_t>(ctx_);
}

std::unique_ptr<task_t> measure_resources(node_dispatcher_test_context& ctx_) {
    return std::make_unique<measure_resources_t>(ctx_);
}

std::unique_ptr<task_t> add_random_resources(node_dispatcher_test_context& ctx_) {
    return std::make_unique<add_random_resources_t>(ctx_);
}

std::unique_ptr<task_t> remove_random_edges(node_dispatcher_test_context& ctx_) {
    return std::make_unique<remove_random_edges_t>(ctx_);
}

