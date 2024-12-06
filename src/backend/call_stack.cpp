#include "call_stack.h"

#include <iostream>
#include <limits>
#include <cstddef>

namespace parsery::backend {

call_stack::call_stack() {
    m_calls.reserve(32);
}

void call_stack::init() {
    m_calls.clear();
    m_pop_count = 0;
}

void call_stack::reset() {
    m_calls.clear();
    m_calls.shrink_to_fit();
    m_pop_count = 0;
}

bool call_stack::closes_cycle(const std::shared_ptr<graph::node>& next) const {
    auto first_found = m_calls.rend();
    auto second_found = m_calls.rend();

    for (auto rit = m_calls.rbegin(); rit != m_calls.rend(); ++rit) {
        if (rit->node == next) {
            if (first_found == m_calls.rend()) {
                first_found = rit;
            } else {
                second_found = rit;

                auto current_count = m_pop_count;
                auto first_count = first_found->pop_count;
                auto second_count = second_found->pop_count;

                return abs_delta(current_count, first_count) == abs_delta(first_count, second_count);
            }
        }
    }

    return false;
}

void call_stack::push_call(const std::shared_ptr<graph::node>& call) {
    m_calls.push_back({call, m_pop_count});
}

void call_stack::pop_call() {
    m_calls.pop_back();
    m_pop_count++;
}

std::shared_ptr<graph::node> call_stack::head() const {
    if (m_calls.empty()) {
        return nullptr;
    } else {
        return m_calls.rbegin()->node;
    }
}

size_t call_stack::abs_delta(const size_t a, const size_t b) const {
    if (a < b) {
        return b - a;
    } else {
        return std::numeric_limits<size_t>::max() - a + b + 1;
    }
}

}

