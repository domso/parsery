#pragma once

#include <memory>
#include <string>

#include "syntax_element.h"
#include "graph/node.h"

namespace parser::frontend {

class syntax_converter {
public:
    std::shared_ptr<graph::node> convert(const syntax_element& element);
private:
    typedef std::pair<std::shared_ptr<graph::node>, std::shared_ptr<graph::node>> syntax_element_fragment;

    void remove_nops(const std::shared_ptr<graph::node>& root) const;
    syntax_element_fragment syntax_element_to_fragment(const syntax_element& element);
    unsigned char escape_char(const std::string t) const;

    int m_fragment_counter = 0;
};

}
