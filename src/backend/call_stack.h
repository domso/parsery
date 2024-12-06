#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "graph/node.h"

namespace parsery::backend {

class call_stack {
public:
    call_stack();

    void init();
    void reset();

    bool closes_cycle(const std::shared_ptr<graph::node>& next) const;

    void push_call(const std::shared_ptr<graph::node>& call);
    void pop_call();

    std::shared_ptr<graph::node> head() const;
private:
    size_t abs_delta(const size_t a, const size_t b) const;
    struct call_item {
        std::shared_ptr<graph::node> node;
        size_t pop_count;        
    };

    std::vector<call_item> m_calls;
    size_t m_pop_count;
};

}
