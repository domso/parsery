#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "graph/node.h"

namespace parser::backend {

class node_stack {
public:
    node_stack();

    void init(const std::string_view& text, const std::shared_ptr<graph::node>& root);

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
        for (const auto& item : m_nodes) {
            call(item.previous_node);
        }
    }
private:
    struct node_stack_item {
        size_t taken_path;
        std::shared_ptr<graph::node> previous_node;
        std::string_view::const_iterator text_position;
        std::string_view::const_iterator max_reached_text_position;

        bool operator==(const node_stack_item& other) const;
    };

    std::vector<node_stack_item> m_nodes;
    std::string_view::const_iterator m_max_reached_text_position;
};

}
