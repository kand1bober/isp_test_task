#pragma once

#include <iostream>
#include <list>
#include <unordered_map>
#include <cstdint>
#include <vector>

struct node_t {
    std::vector<int> connected_;
    int32_t parent_ = 0; // part from DSU algorythm
    int32_t volume_ = 0;

    node_t(int32_t parent) : parent_(parent) {}
};


class dsu {
private:
    std::vector<node_t> nodes_;

    //-------- tree functions --------
    int32_t find_set(int32_t n) {
        while (n != nodes_[n].parent_) {
            n = nodes_[n].parent_ = nodes_[nodes_[n].parent_].parent_; // path compression
            // n = nodes_[n].parent_;
        }
        return n;
    }

    void union_set(int32_t a, int32_t b) {
        a = find_set(a);
        b = find_set(b);

        if (a != b) {
            nodes_[b].parent_ = a; // можно добавить union by rank при желании
        }
    }

public:
    //-------- tree functions --------
    void make_set() {
        nodes_.push_back(node_t(nodes_.size()));
    }

    void make_minimum_spanning_tree() {
        // init DSU
        for (int32_t i = 0; i < nodes_.size(); ++i) {
            nodes_[i].parent_ = i;
        }

        // make MST        
        for (int32_t u = 0; u < nodes_.size(); ++u) {
            for (int32_t v : nodes_[u].connected_) {
                union_set(u, v);
            }
        }

        // balance the amount of the shared resource
        int32_t n = nodes_.size();

        std::vector<int64_t> sum(n, 0);
        std::vector<int32_t> count(n, 0);

        // 1. compressing paths
        for (int32_t i = 0; i < n; ++i) {
            nodes_[i].parent_ = find_set(i);
        }

        // 2. count the amounts and sizes
        for (int32_t i = 0; i < n; ++i) {
            int32_t root = nodes_[i].parent_;
            sum[root] += nodes_[i].volume_;
            count[root] += 1;
        }

        // 3. write the average
        for (int32_t i = 0; i < n; ++i) {
            int32_t root = nodes_[i].parent_;
            nodes_[i].volume_ = sum[root] / count[root];
        }
    }

    //-------- edges functions --------
    void add_connection(int32_t a, int32_t b) {
        nodes_[a].connected_.push_back(b);
        nodes_[b].connected_.push_back(a);        
    }

    void remove_connection(int32_t a, int32_t b) {
        nodes_[a].connected_.erase(nodes_[a].connected_.begin() + b);
        nodes_[b].connected_.erase(nodes_[b].connected_.begin() + a);
    }

    //-------- resource handling --------
    void add_resource(int32_t node_num, int32_t volume) {
        if (volume >= 0)
            nodes_[node_num].volume_ += volume;
        //TODO: 
    }

    int32_t check_resource(int32_t node_num) {
        if (node_num >= 0 && node_num < nodes_.size()) {
            return nodes_[node_num].volume_; 
        }
        return -1;
    }
};
