#include "parse_stack.h"

#include <iostream>

namespace parser {
parse_stack::parse_stack() {
    m_nodes.reserve(32);
    m_calls.reserve(32);
}

void parse_stack::init(const std::string& text, const std::shared_ptr<parse_graph>& root) {
    m_nodes.clear();
    m_calls.clear();
    m_nodes.push_back({0, root, text.begin(), text.begin()});
}

std::shared_ptr<parse_graph> parse_stack::current() const {
    std::shared_ptr<parse_graph> current;

    if (!m_nodes.empty()) {
        auto& item = *m_nodes.rbegin();
        
        if (item.previous_node != nullptr && item.taken_path < item.previous_node->degree()) {
            item.previous_node->child(item.taken_path, [&](const std::shared_ptr<parse_graph>& child) {
                current = child;
            });
        }
    }

    return current;
}

std::shared_ptr<parse_graph> parse_stack::last() const {
    std::shared_ptr<parse_graph> current;

    if (!m_nodes.empty()) {
        auto& item = *m_nodes.rbegin();
        
        if (item.previous_node != nullptr) {
            current = item.previous_node;
        }
    }

    return current;
}

void parse_stack::branch_to_next() {
    if (!m_nodes.empty()) {
        auto& item = *m_nodes.rbegin();
        item.taken_path++;                
    }
}

void parse_stack::push_node(const parse_stack_item& item) {
    m_nodes.push_back(item);
}
void parse_stack::pop_node() {
    m_nodes.pop_back();
}
void parse_stack::push_call(const std::shared_ptr<parse_graph>& call) {
    m_calls.push_back(call);
}
void parse_stack::pop_call() {
    m_calls.pop_back();
}
std::shared_ptr<parse_graph> parse_stack::current_call() const {
    if (m_calls.empty()) {
        return nullptr;
    } else {
        return *m_calls.rbegin();
    }
}
bool parse_stack::closes_cycle(const std::string& next, const std::string::const_iterator& m_it, const std::string::const_iterator& m_max_it) const {
    auto first_found = m_nodes.rend();
    auto second_found = m_nodes.rend();
    for (auto rit = m_nodes.rbegin(); rit != m_nodes.rend(); ++rit) {
        if (auto prev_next = rit->previous_node->local.get<call>()) {
            if (rit->taken_path == 0 && **prev_next == next && m_it == rit->text_position && m_max_it == rit->max_reached_text_position) {
                if (first_found == m_nodes.rend()) {
                    first_found = rit;
                } else {
                    second_found = rit;
    
                    return std::equal(std::next(first_found), std::next(second_found), m_nodes.rbegin(), std::next(first_found));
                }
            }
        }

        if (rit->text_position < m_it) {
            return false;
        }
    }

    return false;
}
bool parse_stack::parse_stack_item::operator==(const parse_stack_item& other) const {
    return 
        taken_path == other.taken_path &&
        previous_node == other.previous_node &&
        text_position == other.text_position &&
        max_reached_text_position == other.max_reached_text_position;
}
}

