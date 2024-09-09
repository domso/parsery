#include "rule_importer.h"

namespace parsery::frontend {

std::shared_ptr<graph::node> rule_importer::import(const std::string& name, const std::string& rule) {
    auto parse_node = m_generator.generate(rule);
    return m_converter.convert(parse_node);
}

}
