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
    void pretty_print(const std::string& text) const;
private:
    std::vector<parse_stack_item> m_nodes;
    std::vector<std::shared_ptr<parse_graph>> m_calls;
};
}
