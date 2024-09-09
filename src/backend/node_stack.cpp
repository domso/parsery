#include "node_stack.h"

#include <cassert>

namespace parser::backend {

node_stack::node_stack() {
    m_nodes.reserve(32);
}

void node_stack::init(const std::string_view& text, const std::shared_ptr<graph::node>& root) {
    m_nodes.clear();
    m_nodes.push_back({0, root, text.begin()});
    m_max_reached_text_position = text.begin();
}

std::shared_ptr<graph::node> node_stack::head() const {
    std::shared_ptr<graph::node> current;

    if (!m_nodes.empty()) {
        auto& item = *m_nodes.rbegin();
        
        if (item.previous_node != nullptr && item.taken_path < item.previous_node->degree()) {
            item.previous_node->child(item.taken_path, [&](const std::shared_ptr<graph::node>& child) {
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
    return m_nodes.rbegin()->text_position;    
}

void node_stack::branch_to_next() {
    if (!m_nodes.empty()) {
        auto& item = *m_nodes.rbegin();
        item.taken_path++;                
    }
}

void node_stack::push_node(
    const size_t taken_path, 
    const std::shared_ptr<graph::node>& previous_node, 
    const std::string_view::const_iterator& text_position
) {
    m_max_reached_text_position = std::max(m_max_reached_text_position, text_position);
    m_nodes.push_back({taken_path, previous_node, text_position, m_max_reached_text_position});
}

void node_stack::pop_node() {
    m_nodes.pop_back();
}
bool node_stack::closes_cycle(
    const std::string_view& next,
    const std::string_view::const_iterator& text_position
) const {
    auto first_found = m_nodes.rend();
    auto second_found = m_nodes.rend();
    for (auto rit = m_nodes.rbegin(); rit != m_nodes.rend(); ++rit) {
        if (auto prev_next = rit->previous_node->local.get<graph::call>()) {
            if (rit->taken_path == 0 && **prev_next == next && text_position == rit->text_position && m_max_reached_text_position == rit->max_reached_text_position) {
                if (first_found == m_nodes.rend()) {
                    first_found = rit;
                } else {
                    second_found = rit;
    
                    return std::equal(std::next(first_found), std::next(second_found), m_nodes.rbegin(), std::next(first_found));
                }
            }
        }

        if (rit->text_position < text_position) {
            return false;
        }
    }

    return false;
}
bool node_stack::node_stack_item::operator==(const node_stack_item& other) const {
    return 
        taken_path == other.taken_path &&
        previous_node == other.previous_node &&
        text_position == other.text_position &&
        max_reached_text_position == other.max_reached_text_position;
}
}

