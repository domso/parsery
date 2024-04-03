#pragma once

#include <unordered_map>

#include "node.h"
#include "node_generator.h"
#include "node_converter.h"
#include "parse_stack.h"

namespace parser {
class parser {
public:
    void add_rule(const std::string& name, const std::string& rule);
    void add_top_rule(const std::string& name, const std::string& rule);
    void parse(const std::string& text);
private:
    bool accepts(const std::shared_ptr<parse_graph>& node, const unsigned char cmp) const;
    bool check_current_node(const std::shared_ptr<parse_graph>& current, const std::string& text);
    bool backtrack_to_previous_node();
    bool sequencer(const std::string text);

    parse_stack m_stack;
    std::string::const_iterator m_it;
    std::string::const_iterator m_max_it;

    node_converter m_converter;
    node_generator m_generator;
    std::unordered_map<std::string, std::shared_ptr<parse_graph>> m_nested_rules;
    std::shared_ptr<parse_graph> m_top_rule;
};
}
