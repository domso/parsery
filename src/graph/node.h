#pragma once

#include <string>

#include "type_union.h"
#include "generic_node.h"

namespace parser::graph {

struct root {
    typedef int type;
    static constexpr auto label = "root";
};

struct leaf {
    typedef int type;
    static constexpr auto label = "leaf";
};

struct nop {
    typedef int type;
    static constexpr auto label = "nop";
};

struct call {
    typedef std::string type;
    static constexpr auto label = "call";
};

struct join {
    typedef std::string type;
    static constexpr auto label = "join";
};

struct character {
    typedef char type;
    static constexpr auto label = "character";
};

struct string {
    typedef std::string type;
    static constexpr auto label = "string";
};

struct range {
    typedef std::pair<unsigned char, unsigned char> type;
    static constexpr auto label = "range";
};

typedef type_union<root, leaf, join, nop, call, character, range, string> concrete_node_type;
typedef type_union<> concrete_edge_type;

typedef generic_node<
    concrete_node_type,
    concrete_edge_type
> node;

}
