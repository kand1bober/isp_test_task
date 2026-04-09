#pragma once

#include <iostream>
#include <list>
#include <unordered_map>
#include <cstdint>
#include <vector>

enum node_state_t : uint8_t {
    kLeader = 0,
    kFollower = 1,
    kSolo = 2,
};

class node_t {
private: 
    int volume_ = 0;
    node_state_t state_ = kSolo;

    std::vector<node_t*> connected_;

    node_t* leader_ = nullptr;

public:
    node_t() = default;

    inline void show_volume() {
        std::cout << "volume: " << volume_ << std::endl;
    }

    inline void connect(node_t& node) {
        connected_.push_back(&node);
        node.connected_.push_back(this);

        if (state_ == kSolo && node.state_ == kSolo) {
            state_ = kLeader;
            volume_ += node.volume_;
            leader_ = this;

            node.state_ = kFollower;
            node.leader_ = this;

            //TODO: пересчет обьема у мн-ва
        }
        
        else if (state_ == kSolo && node.state_ == kLeader) {
            state_ = kFollower;
            leader_ = &node;

            node.volume_ += volume_;
            
            //TODO: пересчет обьема у мн-ва
        }
        else if (state_ == kLeader && node.state_ == kSolo) {
            volume_ += node.volume_;

            node.state_ = kFollower;
            node.leader_ = this;

            //TODO: пересчет обьема у мн-ва
        }

        else if (state_ == kSolo && node.state_ == kFollower) {
            state_ = kFollower;
            leader_ = node.leader_;
            
            leader_->volume_ += volume_;

            //TODO: пересчет обьема у мн-ва
        }
        else if (state_ == kFollower && node.state_ == kSolo) {
            leader_->volume_ += node.volume_;

            node.state_ = kFollower;
            node.leader_ = leader_;

            //TODO: пересчет обьема у мн-ва
        }

        else if (state_ == kFollower && node.state_ == kLeader) {
            // consider which leader has more followers, 
            // change the the smaller set of nodes 
            if (leader_->connected_.size() > node.connected_.size()) {
                node.leader_ = leader_;
                node.state_ = kFollower;
                for (auto tmp_node : node.connected_) {
                    tmp_node->leader_ = leader_; 
                }
            }
            else {
                leader_->leader_ = &node;
                leader_->state_ = kFollower;
                for (auto tmp_node : leader_->connected_) {
                    tmp_node->leader_ = &node;
                }
            }

            //TODO: пересчет обьема у мн-ва
        }
        else if (state_ == kLeader && node.state_ == kFollower) {
            // consider which leader has more followers, 
            // change the the smaller set of nodes 
            if (connected_.size() > node.leader_->connected_.size()) {
                node.leader_->leader_ = this;
                node.leader_->state_ = kFollower;
                for (auto tmp_node : node.leader_->connected_) {
                    tmp_node->leader_ = this;
                }
            }
            else {
                leader_ = node.leader_;
                state_ = kFollower;
                for (auto tmp_node : connected_) {
                    tmp_node->leader_ = node.leader_;
                }
            }

            //TODO: пересчет обьема у мн-ва
        }
        
        else if (state_ == kLeader && node.state_ == kLeader) {
            // consider which leader has more followers, 
            // change the the smaller set of nodes 
            if (connected_.size() > node.connected_.size()) {
                node.state_ = kFollower;
                node.leader_ = this;
                for (auto tmp_node : node.connected_) {
                    tmp_node->leader_ = this;
                }
            }

            //TODO: пересчет обьема у мн-ва
        }
    }
};
