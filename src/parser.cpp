#include "parser.h"
#include <chrono>
#include <iostream>

namespace parsery {
void parser::add_rule(const std::string& name, const std::string& rule) {
    m_nested_rules[name] = m_importer.import(name, rule);
}
void parser::add_top_rule(const std::string& name, const std::string& rule) {
    auto node = m_importer.import(name, rule);
    m_nested_rules[name] = node;
    m_top_rule = node;
}

bool parser::accepts(const std::shared_ptr<graph::node>& node, const unsigned char cmp) const {
    if (auto c = node->local.get<graph::character>()) {
        return (**c == cmp);
    }
    if (auto c = node->local.get<graph::range>()) {
        return ((**c).first <= cmp && cmp <= (**c).second);
    }

    return false;
}

bool parser::check_current_node(const std::shared_ptr<graph::node>& current, const std::string& text) {
    if (current->local.is<graph::root>() || current->local.is<graph::nop>() || current->local.is<graph::join>()) {
        m_node_stack.push_node(0, current, m_node_stack.position());
    } else if (current->local.is<graph::leaf>()) {
        auto call = m_call_stack.head();
        if (call == nullptr) {
            // current path leads to final leaf node
            if (m_node_stack.position() != text.end()) {
                m_node_stack.branch_to_next();
            } else {
                return true;
            }
        } else {
            m_node_stack.push_node(0, current, m_node_stack.position());
            m_node_stack.push_node(0, call, m_node_stack.position());

            m_call_stack.pop_call();
        }
    } else if (auto next = current->local.get<graph::call>()) {
        bool found = m_node_stack.closes_cycle(**next, m_node_stack.position());

        if (found) {
            m_node_stack.branch_to_next();
        } else {
            current->child(0, [&](const std::shared_ptr<graph::node>& child) {
                m_call_stack.push_call(child);
            });

            m_node_stack.push_node(0, current, m_node_stack.position());
            m_node_stack.push_node(0, m_nested_rules[**next], m_node_stack.position());
        }
    } else if (auto character_sequence = current->local.get<graph::string>()) {
        auto& cmp = **character_sequence;
        auto distance = std::distance(m_node_stack.position(), text.end());
        if (distance >= cmp.length() && std::equal(m_node_stack.position(), m_node_stack.position() + cmp.length(), cmp.begin(), cmp.end())) {
            m_node_stack.push_node(0, current, m_node_stack.position() + cmp.length());
        } else {
            m_node_stack.branch_to_next();
        }
    } else if (m_node_stack.position() < text.end() && accepts(current, *m_node_stack.position())) {
        m_node_stack.push_node(0, current, m_node_stack.position() + 1);
    } else {
        m_node_stack.branch_to_next();
    }

    return false;
}

bool parser::backtrack_to_previous_node() {
    auto node_top = m_node_stack.previous();

    if (node_top == nullptr) {
        return false;
    }

    if (node_top->local.is<graph::join>()) {
        m_call_stack.push_call(node_top);
    }
    if (node_top->local.is<graph::call>()) {
        m_call_stack.pop_call();
    }

    m_node_stack.pop_node();
    m_node_stack.branch_to_next();

    return true;
}

bool parser::sequencer(const std::string text) {
    m_node_stack.init(text, m_top_rule);
    m_call_stack.init();

    while (true) {
        auto current = m_node_stack.head();

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
