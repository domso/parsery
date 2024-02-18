#pragma once

#include <memory>
#include <string>

#include "node.h"
#include "parse_graph.h"

namespace parser {
class node_converter {
public:
    std::shared_ptr<parse_graph> convert(const parser::node& node);
private:
    typedef std::pair<std::shared_ptr<parse_graph>, std::shared_ptr<parse_graph>> node_fragment;

    void remove_nops(const std::shared_ptr<parse_graph>& root) const;
    node_fragment node_to_fragment(const parser::node& node);

    int m_fragment_counter = 0;
};
}

