#include "node_stack.h"

#include <cassert>

namespace parsery::backend {

node_stack::node_stack() {
    m_nodes.reserve(32);
}

void node_stack::init(const std::string_view& text, const std::shared_ptr<graph::node>& root) {
    m_nodes.clear();
    m_nodes.push_back({root});
    m_taken_path = 0;
    m_text_position = text.begin();
}

std::shared_ptr<graph::node> node_stack::head() const {
    std::shared_ptr<graph::node> current;

    if (!m_nodes.empty()) {
        auto& item = *m_nodes.rbegin();
        
        if (item.previous_node != nullptr && m_taken_path < item.previous_node->degree()) {
            item.previous_node->child(m_taken_path, [&](const std::shared_ptr<graph::node>& child) {
                current = child;
            });
        }
    }

    return current;
}

std::shared_ptr<graph::node> node_stack::previous() const {
    std::shared_ptr<graph::node> current;

    if (!m_nodes.empty()) {
        auto& item = *m_nodes.rbegin();
        
        if (item.previous_node != nullptr) {
            current = item.previous_node;
        }
    }

    return current;
}

const std::string_view::const_iterator& node_stack::position() const {
    assert(!m_nodes.empty());
    return m_text_position;
}

void node_stack::branch_to_next() {
    if (!m_nodes.empty()) {
        m_taken_path++;
    }
}

void node_stack::push_node(
    const size_t taken_path, 
    const std::shared_ptr<graph::node>& previous_node, 
    const std::string_view::const_iterator& text_position
) {
    m_nodes.push_back({previous_node});
    m_taken_path = taken_path;
    assert(taken_path == 0);
    m_text_position = text_position;
}

void node_stack::pop_node() {
    auto next = m_nodes.rbegin()->previous_node;
    m_nodes.pop_back();

    if (next->local.is<graph::range, graph::character>()) {
        m_text_position--;
    }
    if (next->local.is<graph::string>()) {
        if (const auto& seq = next->local.get<graph::string>()) {
            m_text_position -= (**seq).length();
        }
    }

    if (m_nodes.empty()) {
        m_taken_path = 0;
    } else {
        if (auto index = m_nodes.rbegin()->previous_node->index_of_child(next)) {
            m_taken_path = *index;
        } else {
            // node is not actually child, but was taken as such --> it must be a call
            m_taken_path = 0;
        }
    }
}
bool node_stack::closes_cycle(
    const std::string_view& next,
    const std::string_view::const_iterator& text_position
) const {
    auto first_found = m_nodes.rend();
    auto second_found = m_nodes.rend();
    auto current_path = m_taken_path;
    auto current_text_position = m_text_position;

    std::shared_ptr<graph::node> child;

    for (auto rit = m_nodes.rbegin(); rit != m_nodes.rend(); ++rit) {
        if (child) {
            if (auto index = rit->previous_node->index_of_child(child)) {
                current_path = *index;
            }
        }

        if (auto prev_next = rit->previous_node->local.get<graph::call>()) {
            if (current_path == 0 && **prev_next == next && text_position == current_text_position) {
                if (first_found == m_nodes.rend()) {
                    first_found = rit;
                } else {
                    second_found = rit;
    
                    return std::equal(std::next(first_found), std::next(second_found), m_nodes.rbegin(), std::next(first_found));
                }
            }
        }

        if (current_text_position < text_position) {
            return false;
        }
        child = rit->previous_node;

        if (child->local.is<graph::range, graph::character>()) {
            current_text_position--;
        }
        if (child->local.is<graph::string>()) {
            if (const auto& seq = child->local.get<graph::string>()) {
                current_text_position -= (**seq).length();
            }
        }
    }

    return false;
}
bool node_stack::node_stack_item::operator==(const node_stack_item& other) const {
    return 
        previous_node == other.previous_node;
}
}

