#include "node_converter.h"

namespace parser {
std::shared_ptr<parse_graph> node_converter::convert(const parser::node& node) {
    m_fragment_counter = 0;

    auto [start, end] = node_to_fragment(node);
    remove_nops(start);
    start->untag_nested_children();

    start->sort_children([&](const parse_graph& left, const parse_graph& right) {
        if (right.local.is<leaf>()) {
            return true;;
        }
        return false;
    });
    start->untag_nested_children();

    return start;
}

void node_converter::remove_nops(const std::shared_ptr<parse_graph>& root) const {
    if (!root->tag()) {
        auto current = root;

        std::vector<std::shared_ptr<parse_graph>> remove_childs;

        current->tag() = true;
        current->for_each_child([&](std::shared_ptr<parse_graph>& child) {
            remove_nops(child);
        });

        current->for_each_child([&](std::shared_ptr<parse_graph>& child) {
            if (child->local.is<nop>()) {
                remove_childs.push_back(child);
                child->for_each_child([&](std::shared_ptr<parse_graph>& grandchild) {
                    current->make_edge(grandchild);
                });
            }
        });

        for (const auto i : remove_childs) {
            current->remove_edge(i);
        }
    }
}

node_converter::node_fragment node_converter::node_to_fragment(const parser::node& node) {
    auto start = std::make_shared<parse_graph>();
    auto end = std::make_shared<parse_graph>();
    std::shared_ptr<parse_graph> current;

    if (m_fragment_counter == 0) {
        start->local.cast_to<root>(m_fragment_counter);
        end->local.cast_to<leaf>(m_fragment_counter);
    } else {
        start->local.cast_to<nop>(m_fragment_counter);
        end->local.cast_to<nop>(m_fragment_counter + 1);
    }

    m_fragment_counter += 2;

    if (node.is_range()) {
        auto range_node = std::make_shared<parse_graph>();
        range_node->local.cast_to<range>({node.children[0].text[0], node.children[2].text[0]});
        start->make_edge(range_node);
        range_node->make_edge(end);

        return {start, end};
    }
    if (node.is_branch()) {
        auto [left_start, left_end] = node_to_fragment(node.children[0]);
        auto [right_start, right_end] = node_to_fragment(node.children[2]);

        start->make_edge(left_start);
        start->make_edge(right_start);

        left_end->make_edge(end);
        right_end->make_edge(end);

        return {start, end};

    }
    if (node.is_optional()) {
        auto [left_start, left_end] = node_to_fragment(node.children[0]);

        start->make_edge(left_start);
        start->make_edge(end);
        left_end->make_edge(left_start);
        left_end->make_edge(end);

        return {start, end};
    }
    if (node.is_link()) {
        auto link_node = std::make_shared<parse_graph>();
        auto join_node = std::make_shared<parse_graph>();

        link_node->local.cast_to<call>(node.link);
        join_node->local.cast_to<join>(node.link);

        start->make_edge(link_node);
        link_node->make_edge(join_node);
        join_node->make_edge(end);

        return {start, end};
    }
    if (node.text.length() == 0) {
        current = start;

        for (const auto& child : node.children) {
            auto [child_start, child_end] = node_to_fragment(child);

            current->make_edge(child_start);
            current = child_end;
        }

        current->make_edge(end);

        return {start, end};
    }

    current = start;
    bool escaped = false;
    std::string cleaned_text;
    for (auto c : node.text) {
        if (escaped) {
            escaped = false;
            cleaned_text += c;
        } else {
            if (c != '\\') {
                cleaned_text += c;
            } else {
                escaped = true;
            }
        }
    }

    if (cleaned_text.length() == 1) {
        current->make_edge([&](
            std::shared_ptr<parse_graph>& target
        ) {
            target->local.cast_to<character>(cleaned_text.at(0));
            current = target;
        });
    } else {
        current->make_edge([&](
            std::shared_ptr<parse_graph>& target
        ) {
            target->local.cast_to<string>(cleaned_text);
            current = target;
        });
    }

    current->make_edge(end);

    return {start, end};
}
}
