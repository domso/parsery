#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "graph/node.h"

namespace parser::backend {

class call_stack {
public:
    call_stack();

    void init();
    void push_call(const std::shared_ptr<graph::node>& call);
    void pop_call();

    std::shared_ptr<graph::node> head() const;
private:
    std::vector<std::shared_ptr<graph::node>> m_calls;
};

}
