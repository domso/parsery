#pragma once

#include <memory>
#include <string>
#include <vector>

#include "node.h"
#include "parse_graph.h"

namespace parser {
class parse_stack {
public:
    parse_stack();

    void init(const std::string& text, const std::shared_ptr<parse_graph>& root);

    std::shared_ptr<parse_graph> current() const;
    std::shared_ptr<parse_graph> last() const;

    void branch_to_next();

    struct parse_stack_item {
        size_t taken_path;
        std::shared_ptr<parse_graph> previous_node;
        std::string::const_iterator text_position;
        std::string::const_iterator max_reached_text_position;

        bool operator==(const parse_stack_item& other) const;
    };

    void push_node(const parse_stack::parse_stack_item& item);
    void pop_node();
    void push_call(const std::shared_ptr<parse_graph>& call);
    void pop_call();

    std::shared_ptr<parse_graph> current_call() const;
    bool closes_cycle(const std::string& next, const std::string::const_iterator& m_it, const std::string::const_iterator& m_max_it) const;

    void print() const;
    template<typename T_open_call, typename T_close_call, typename T_data_call>
    void scan_result(const std::string& text, const T_open_call& open_call, const T_close_call& close_call, const T_data_call& data_call) const {
        auto it = text.begin();
        auto first = it;

        for (const auto& item : m_nodes) {
            std::shared_ptr<parse_graph> current;
            item.previous_node->child(item.taken_path, [&](const std::shared_ptr<parse_graph>& child) {
                current = child;
            });

            if (item.previous_node->local.is<root, leaf>()) {
                continue;
            }

            if (item.previous_node->local.is<range, character>() && it != text.end()) {
                ++it;
            }
            if (item.previous_node->local.is<string>()) {
                if (const auto& seq = item.previous_node->local.get<string>()) {
                    std::advance(it, std::min(static_cast<size_t>(std::distance(it, text.end())), (**seq).length()));
                }
            }
            if (item.previous_node->local.is<join, call>()) {
                if (first != it) {
                    data_call(std::string_view(first.operator->(), it - first));
                    first = it;
                }
            }
            if (auto scope = item.previous_node->local.get<call>()) {
                open_call(**scope);
            }
            if (auto scope = item.previous_node->local.get<join>()) {
                close_call(**scope);
            }
        }
        if (first != it) {
            data_call(std::string_view(first.operator->(), it - first));
        }
    }
private:
    std::vector<parse_stack_item> m_nodes;
    std::vector<std::shared_ptr<parse_graph>> m_calls;
};
}
