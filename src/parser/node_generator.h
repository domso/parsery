#pragma once

#include <vector>
#include <string>

#include "node.h"

namespace parser {
    class node_generator {
    public:
        node generate(const std::string& text) const;
    private:
        std::string extend_star(const std::string& text, const int num) const;
        std::string extend_or(const std::string& text, const int num) const;
        std::string extend_range(const std::string& text, const int num) const;
        std::string extend_label(const std::string& text, const int num) const;
        
        void reduce_node(node& n) const;
        void split_node(node& n) const;
        void split_block_node(node& n) const;
        void link_node(node& n) const;
        void clean_node(node& n) const;
    };
}
