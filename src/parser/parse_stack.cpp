#include "parse_stack.h"

#include <iostream>

namespace parser {
parse_stack::parse_stack() {
    m_nodes.reserve(32);
    m_calls.reserve(32);
}

void parse_stack::init(const std::string& text, const std::shared_ptr<parse_graph>& root) {
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
void parse_stack::print() const {
    int level = 0;
    for (const auto& item : m_nodes) {
        std::shared_ptr<parse_graph> current;
        item.previous_node->child(item.taken_path, [&](const std::shared_ptr<parse_graph>& child) {
            current = child;
        });

        if (item.previous_node->local.is<join>()) {
            level -= 4;
        }
        level = std::max(0, level);
        std::cout << std::string(level, ' ') << item.taken_path << " " << item.previous_node->local.to_string() << " " << std::endl;

        if (item.previous_node->local.is<call>()) {
            level += 4;
        }
    }
}
void parse_stack::pretty_print(const std::string& text) const {
    int level = 0;
    auto it = text.begin();
    std::string current_word;
    for (const auto& item : m_nodes) {
        std::shared_ptr<parse_graph> current;
        item.previous_node->child(item.taken_path, [&](const std::shared_ptr<parse_graph>& child) {
            current = child;
        });

        if (item.previous_node->local.is<root>() || item.previous_node->local.is<leaf>()) {
            continue;
        }

        if (auto scope = item.previous_node->local.get<call>()) {
            if (current_word != "") {
                std::cout << std::string(level, ' ') << "'" << current_word << "'" <<  "\n";
                current_word = "";
            }
            std::cout << std::string(level, ' ') << "<" << **scope << ">\n";
            level += 4;
        }
        if (item.previous_node->local.is<character>() && it != text.end()) {
            if (*it != '\n') {
                current_word += *it;
            }
            ++it;
        }
        if (item.previous_node->local.is<string>()) {
            if (const auto& seq = item.previous_node->local.get<string>()) {
                for (auto t : **seq) {
                    if (it != text.end()) {
                        if (*it != '\n') {
                            current_word += *it;
                        }
                        ++it;
                    }
                }
            }
        }
        if (item.previous_node->local.is<range>() && it != text.end()) {
            if (*it != '\n') {
                current_word += *it;
            }
            ++it;
        }
        if (auto scope = item.previous_node->local.get<join>()) {
            if (current_word != "") {
                std::cout << std::string(level, ' ') << "'" << current_word << "'" <<  "\n";
                current_word = "";
            }
            level -= 4;
            level = std::max(0, level);
            std::cout << std::string(level, ' ') <<  "</" << **scope << ">\n";
        }
    }
    if (current_word != "") {
        std::cout << std::string(level, ' ') << "'" << current_word << "'" <<  "\n";
        current_word = "";
    }
}
bool parse_stack::parse_stack_item::operator==(const parse_stack_item& other) const {
    return 
        taken_path == other.taken_path &&
        previous_node == other.previous_node &&
        text_position == other.text_position &&
        max_reached_text_position == other.max_reached_text_position;
}
}

