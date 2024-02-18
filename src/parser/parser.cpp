#include "parser.h"
#include <chrono>
#include <iostream>

namespace parser {
void parser::add_rule(const std::string& name, const std::string& rule) {
    auto parse_node = m_generator.generate(rule);
    auto rule_graph = m_converter.convert(parse_node);

    m_nested_rules[name] = rule_graph;

    parse_node.print();

    std::cout << rule_graph->to_string() << std::endl;
}
void parser::add_top_rule(const std::string& name, const std::string& rule) {
    auto parse_node = m_generator.generate(rule);
    auto rule_graph = m_converter.convert(parse_node);

    m_nested_rules[name] = rule_graph;
    m_top_rule = rule_graph;

    parse_node.print();
    std::cout << m_top_rule->to_string() << std::endl;
}
void parser::parse(const std::string& text) {
    auto t1 = std::chrono::steady_clock::now();
    sequencer(text);
    auto t2 = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
    std::cout << duration << std::endl;
    m_stack.pretty_print(text);
}

bool parser::accepts(const std::shared_ptr<parse_graph>& node, const char cmp) const {
    if (auto c = node->local.get<character>()) {
        return (**c == cmp);
    }
    if (auto c = node->local.get<range>()) {
        return ((**c).first <= cmp && cmp <= (**c).second);
    }

    return false;
}

bool parser::check_current_node(const std::shared_ptr<parse_graph>& current, const std::string& text) {
    if (current->local.is<root>() || current->local.is<nop>() || current->local.is<join>()) {
        m_stack.push_node({0, current, m_it, m_max_it});
    } else if (current->local.is<leaf>()) {
        auto call = m_stack.current_call();
        if (call == nullptr) {
            // current path leads to final leaf node
            if (m_it != text.end()) {
                m_stack.branch_to_next();
            } else {
                return true;
            }
        } else {
            m_stack.push_node({0, current, m_it, m_max_it});
            m_stack.push_node({0, call, m_it, m_max_it});

            m_stack.pop_call();
        }
    } else if (auto next = current->local.get<call>()) {
        bool found = m_stack.closes_cycle(**next, m_it, m_max_it);

        if (found) {
            m_stack.branch_to_next();
        } else {
            current->child(0, [&](const std::shared_ptr<parse_graph>& child) {
                m_stack.push_call(child);
            });

            m_stack.push_node({0, current, m_it, m_max_it});
            m_stack.push_node({0, m_nested_rules[**next], m_it, m_max_it});
        }
    } else if (auto character_sequence = current->local.get<string>()) {
        auto& cmp = **character_sequence;
        if (std::equal(m_it, m_it + cmp.length(), cmp.begin(), cmp.end())) {
            m_it += cmp.length();
            m_stack.push_node({0, current, m_it, m_max_it});
        }
    } else if (m_it < text.end() && accepts(current, *m_it)) {
        ++m_it;
        m_stack.push_node({0, current, m_it, m_max_it});
    } else {
        m_stack.branch_to_next();
    }

    return false;
}

bool parser::backtrack_to_previous_node() {
    auto node_top = m_stack.last();

    if (node_top == nullptr) {
        return false;
    }

    if (!(node_top->local.is<root>() || node_top->local.is<nop>() || node_top->local.is<leaf>() || node_top->local.is<call>() || node_top->local.is<join>())) {
        --m_it;
    }

    if (node_top->local.is<join>()) {
        m_stack.push_call(node_top);
    }
    if (node_top->local.is<call>()) {
        m_stack.pop_call();
    }

    m_stack.pop_node();
    m_stack.branch_to_next();

    return true;
}

bool parser::sequencer(const std::string text) {
    m_stack.init(text, m_top_rule);

    m_it = text.begin();
    m_max_it = m_it;

    while (true) {
        m_max_it = std::max(m_max_it, m_it);
        auto current = m_stack.current();

        if (current != nullptr) {
            if (check_current_node(current, text)) {
                return true;
            }
        } else if (!backtrack_to_previous_node()) {
            return false;
        }
    }
    
    return false;
}
}
