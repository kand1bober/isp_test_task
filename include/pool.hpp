#pragma once

#include <iostream>
#include <list>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <algorithm>

struct node_t {
    std::vector<int> connected_;
    int32_t parent_ = 0; // part from DSU algorythm
    int32_t volume_ = 0;

    node_t(int32_t parent) : parent_(parent) {}

    void remove_connection(int32_t id) {
        auto iter = std::find(connected_.begin(), connected_.end(), id);

        // change curr element with last and delete last
        if (iter != connected_.end()) {
            *iter = connected_.back();
            connected_.pop_back();
        }
    }
};


class node_dispatcher {
private:
    std::vector<node_t> nodes_;

    //-------- tree functions --------
    void make_set() {
        nodes_.push_back(node_t(nodes_.size()));
    }
    
    int32_t find_set(int32_t n) {
        while (n != nodes_[n].parent_) {
            n = nodes_[n].parent_ = nodes_[nodes_[n].parent_].parent_; // path compression
        }
        return n;
    }

    void union_set(int32_t a, int32_t b) {
        a = find_set(a);
        b = find_set(b);

        if (a != b) {
            nodes_[b].parent_ = a;
        }
    }

    // make minimum spanning tree out of graph
    void make_mst() {
        // set every node as a parent to themselves
        for (size_t i = 0; i < nodes_.size(); ++i) {
            nodes_[i].parent_ = i;
        }

        // pairwise union for all nodes
        for (size_t u = 0; u < nodes_.size(); ++u) {
            for (int32_t v : nodes_[u].connected_) {
                union_set(u, v);
            }
        }
    }

    // set a representative of MST as a parent for every node in this MST
    void compress_paths() {
        for (size_t i = 0; i < nodes_.size(); ++i) {
            nodes_[i].parent_ = find_set(i);
        }
    }

    // balance the amount of the shared resource in MST
    void balance_mst_shared_resource() {
        std::vector<int64_t> total_volume(nodes_.size(), 0);
        std::vector<int32_t> mst_nodes_count(nodes_.size(), 0);

        for (size_t i = 0; i < nodes_.size(); ++i) {
            int32_t root = nodes_[i].parent_; // find representative of mst this node is in
            total_volume[root] += nodes_[i].volume_; // add volume of this node to total volume of this mst
            mst_nodes_count[root] += 1;
        }

        // calculate and write to each node of mst the average volume of resource
        for (size_t i = 0; i < nodes_.size(); ++i) {
            int32_t root = nodes_[i].parent_;
            nodes_[i].volume_ = total_volume[root] / mst_nodes_count[root];
        }
    }

public:
    void add_node() {
        make_set();
    }

    //-------- tree functions --------
    void calculate_resources() {
        make_mst();
        compress_paths();
        balance_mst_shared_resource();
    }

    //-------- edges functions --------
    void add_connection(int32_t a, int32_t b) {
        nodes_[a].connected_.push_back(b);
        nodes_[b].connected_.push_back(a);        
    }

    void remove_edge(int32_t a, int32_t b) {
        nodes_[a].remove_connection(b);
        nodes_[b].remove_connection(a);
    }

    //-------- resource handling --------
    int32_t add_resource(int32_t node_num, int32_t volume) {
        if (volume >= 0) {
            nodes_[node_num].volume_ += volume;
            return 0;
        }
        else 
            return -1;
    }

    int32_t show_amount_of_resource(int32_t node_num) {
        if (node_num >= 0 && (size_t)node_num < nodes_.size()) {
            return nodes_[node_num].volume_; 
        }
        return -1;
    }
};
