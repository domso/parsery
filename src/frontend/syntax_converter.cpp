#include "syntax_converter.h"

namespace parser::frontend {

std::shared_ptr<graph::node> syntax_converter::convert(const syntax_element& element) {
    m_fragment_counter = 0;

    auto [start, end] = syntax_element_to_fragment(element);
    remove_nops(start);
    start->untag_nested_children();

    start->sort_children([&](const graph::node& left, const graph::node& right) {
        if (right.local.is<graph::leaf>()) {
            return true;;
        }
        return false;
    });
    start->untag_nested_children();

    return start;
}

void syntax_converter::remove_nops(const std::shared_ptr<graph::node>& root) const {
    if (!root->tag()) {
        auto current = root;

        std::vector<std::shared_ptr<graph::node>> remove_childs;

        current->tag() = true;
        current->for_each_child([&](std::shared_ptr<graph::node>& child) {
            remove_nops(child);
        });

        current->for_each_child([&](std::shared_ptr<graph::node>& child) {
            if (child->local.is<graph::nop>()) {
                remove_childs.push_back(child);
                child->for_each_child([&](std::shared_ptr<graph::node>& grandchild) {
                    current->make_edge(grandchild);
                });
            }
        });

        for (const auto i : remove_childs) {
            current->remove_edge(i);
        }
    }
}

syntax_converter::syntax_element_fragment syntax_converter::syntax_element_to_fragment(const syntax_element& element) {
    auto start = std::make_shared<graph::node>();
    auto end = std::make_shared<graph::node>();
    std::shared_ptr<graph::node> current;

    if (m_fragment_counter == 0) {
        start->local.cast_to<graph::root>(m_fragment_counter);
        end->local.cast_to<graph::leaf>(m_fragment_counter);
    } else {
        start->local.cast_to<graph::nop>(m_fragment_counter);
        end->local.cast_to<graph::nop>(m_fragment_counter + 1);
    }

    m_fragment_counter += 2;

    if (element.is_range()) {
        auto range_syntax_element = std::make_shared<graph::node>();
        range_syntax_element->local.cast_to<graph::range>(std::make_pair<unsigned char, unsigned char>(escape_char(element.children[0].text), escape_char(element.children[2].text)));
        start->make_edge(range_syntax_element);
        range_syntax_element->make_edge(end);

        return {start, end};
    }
    if (element.is_branch()) {
        auto [left_start, left_end] = syntax_element_to_fragment(element.children[0]);
        auto [right_start, right_end] = syntax_element_to_fragment(element.children[2]);

        start->make_edge(left_start);
        start->make_edge(right_start);

        left_end->make_edge(end);
        right_end->make_edge(end);

        return {start, end};

    }
    if (element.is_optional()) {
        auto [left_start, left_end] = syntax_element_to_fragment(element.children[0]);

        start->make_edge(left_start);
        start->make_edge(end);
        left_end->make_edge(left_start);
        left_end->make_edge(end);

        return {start, end};
    }
    if (element.is_link()) {
        auto link_syntax_element = std::make_shared<graph::node>();
        auto join_syntax_element = std::make_shared<graph::node>();

        link_syntax_element->local.cast_to<graph::call>(element.link);
        join_syntax_element->local.cast_to<graph::join>(element.link);

        start->make_edge(link_syntax_element);
        link_syntax_element->make_edge(join_syntax_element);
        join_syntax_element->make_edge(end);

        return {start, end};
    }
    if (element.text.length() == 0) {
        current = start;

        for (const auto& child : element.children) {
            auto [child_start, child_end] = syntax_element_to_fragment(child);

            current->make_edge(child_start);
            current = child_end;
        }

        current->make_edge(end);

        return {start, end};
    }

    current = start;
    bool escaped = false;
    std::string cleaned_text;
    for (auto c : element.text) {
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
            std::shared_ptr<graph::node>& target
        ) {
            target->local.cast_to<graph::character>(cleaned_text.at(0));
            current = target;
        });
    } else {
        current->make_edge([&](
            std::shared_ptr<graph::node>& target
        ) {
            target->local.cast_to<graph::string>(cleaned_text);
            current = target;
        });
    }

    current->make_edge(end);

    return {start, end};
}

unsigned char syntax_converter::escape_char(const std::string t) const {
    if (t.length() == 2 && t.at(0) == '\\') {
        return t.at(1);
    }
    return t.at(0);
}

}
