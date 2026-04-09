#pragma once

#include <iostream>
#include <list>
#include <unordered_map>

class pool_t {
private: 
    int volume_ = 0, name_ = -1;

    std::list<pool_t*> connected_;
    using ListIt = typename std::list<pool_t*>::iterator;
    std::unordered_map<int, ListIt> hash_;

public:
    pool_t(int name) : name_(name) {} // ctor

    inline void add_water(int volume) {
        volume_ += volume;
    }

    inline void share_water(int volume) {
        int volume_delta = volume / connected_.size();
        for (auto pool_ptr : connected_) {
            pool_ptr->add_water(volume_delta);
        }
    }

    inline void show_volume() {
        std::cout << "volume: " << volume_ << std::endl;
    }

    inline void connect_pool(const pool_t& pool) {
        hash_[pool.name_] = 
    }

    inline void disconnect_pool(pool_t* pool_ptr) {
        connected_.push_back(pool_ptr);
        pool_ptr->connect_pool(*this);
    }


};
