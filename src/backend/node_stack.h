#pragma once

#include <memory>
#include <iostream>
#include <string_view>
#include <vector>

#include "graph/node.h"

namespace parsery::backend {

class node_stack {
public:
    node_stack();

    void init(const std::string_view& text, const std::shared_ptr<graph::node>& root);
    void reset();

    std::shared_ptr<graph::node> head() const;
    std::shared_ptr<graph::node> previous() const;

    const std::string_view::const_iterator& position() const;

    void branch_to_next();

    void push_node(
        const size_t taken_path, 
        const std::shared_ptr<graph::node>& previous_node, 
        const std::string_view::const_iterator& text_position
    );
    void pop_node();

    bool closes_cycle(
        const std::string_view& next,
        const std::string_view::const_iterator& text_position
    ) const;

    template<typename T_call>
    void for_each(const T_call& call) const {
        std::cout << m_nodes.size() << std::endl;
        std::cout << m_nodes.capacity() << std::endl;
        std::cout << sizeof(node_stack_item) << std::endl;

        int num_char = 0;
        int num_string = 0;
        int num_call = 0;
        int num_join = 0;
        int num_other = 0;

        for (const auto& item : m_nodes) {
            auto node = item.previous_node;
            if (node->local.is<graph::range, graph::character>()) {
                num_char++;
            } else if (node->local.is<graph::string>()) {
                num_string++;
            } else if (auto scope = node->local.get<graph::call>()) {
                num_call++;
            } else if (auto scope = node->local.get<graph::join>()) {
                num_join++;
            } else {
                num_other++;
            }
        }

        std::cout << num_char << std::endl;
        std::cout << num_string << std::endl;
        std::cout << num_call << std::endl;
        std::cout << num_join << std::endl;
        std::cout << num_other << std::endl;

        for (const auto& item : m_nodes) {
            call(item.previous_node);
        }
    }

    void print() {
        if (m_nodes.empty()) {
            return;
    }
        std::cout << "node stack:" << std::endl;
        int n = 0;
        for (auto& item : m_nodes) {
            if (item.previous_node) {
                if (n == m_nodes.size() - 1) {
            std::cout << "    " << " '" << position() << "'"  << std::endl;
                    std::cout << "----" << item.previous_node->to_string_without_children();
                } else {
                    //std::cout << "    " << item.previous_node->to_string_without_children();
                }
            }
        n++;
        }
    }
private:
    struct node_stack_item {
        std::shared_ptr<graph::node> previous_node;

        bool operator==(const node_stack_item& other) const;
    };

    std::vector<node_stack_item> m_nodes;
    size_t m_taken_path;
    std::string_view::const_iterator m_text_position;
};

}
