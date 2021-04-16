#pragma once

#include <vector>
#include <string>

#include "node.h"

namespace parser {
    class node_generator {
    public:
        node generate(const std::string& text);
    private:
        std::string extend_star(const std::string& text, const int num);
        std::string extend_or(const std::string& text, const int num);
        std::string extend_range(const std::string& text, const int num);
        std::string extend_label(const std::string& text, const int num);
        
        void reduce_node(node& n);
        void split_node(node& n);
        void split_block_node(node& n);
    };
}
