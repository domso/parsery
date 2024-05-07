#pragma once

#include <memory>

#include "syntax_generator.h"
#include "syntax_converter.h"

#include "graph/node.h"

namespace parser::frontend {

class rule_importer {
public:
    std::shared_ptr<graph::node> import(const std::string& name, const std::string& rule);
private:
    syntax_converter m_converter;
    syntax_generator m_generator;
};


}
