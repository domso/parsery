#pragma once

#include <unordered_map>

#include "graph/node.h"
#include "backend/call_stack.h"
#include "backend/node_stack.h"

#include "frontend/rule_importer.h"

namespace parser {
class parser {
public:
    void add_rule(const std::string& name, const std::string& rule);
    void add_top_rule(const std::string& name, const std::string& rule);
    template<typename T_open_call, typename T_close_call, typename T_data_call>
    void parse(const std::string& text, const T_open_call& open_call, const T_close_call& close_call, const T_data_call& data_call) {
        sequencer(text);
        scan_result(text, open_call, close_call, data_call);
    }
private:
    template<typename T_open_call, typename T_close_call, typename T_data_call>
    void scan_result(const std::string& text, const T_open_call& open_call, const T_close_call& close_call, const T_data_call& data_call) const {
        auto it = text.begin();
        auto first = it;

        m_node_stack.for_each([&](const std::shared_ptr<graph::node>& node) {
            if (node->local.is<graph::range, graph::character>() && it != text.end()) {
                ++it;
            }
            if (node->local.is<graph::string>()) {
                if (const auto& seq = node->local.get<graph::string>()) {
                    std::advance(it, std::min(static_cast<size_t>(std::distance(it, text.end())), (**seq).length()));
                }
            }
            if (node->local.is<graph::join, graph::call>()) {
                if (first != it) {
                    data_call(std::string_view(first.operator->(), it - first));
                    first = it;
                }
            }
            if (auto scope = node->local.get<graph::call>()) {
                open_call(**scope);
            }
            if (auto scope = node->local.get<graph::join>()) {
                close_call(**scope);
            }
        });

        if (first != it) {
            data_call(std::string_view(first.operator->(), it - first));
        }
    }
    bool accepts(const std::shared_ptr<graph::node>& node, const unsigned char cmp) const;
    bool check_current_node(const std::shared_ptr<graph::node>& current, const std::string& text);
    bool backtrack_to_previous_node();
    bool sequencer(const std::string text);

    frontend::rule_importer m_importer;

    backend::call_stack m_call_stack;
    backend::node_stack m_node_stack;

    std::unordered_map<std::string, std::shared_ptr<graph::node>> m_nested_rules;
    std::shared_ptr<graph::node> m_top_rule;
};

}
