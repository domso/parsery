#include "call_stack.h"

namespace parsery::backend {

call_stack::call_stack() {
    m_calls.reserve(32);
}

void call_stack::init() {
    m_calls.clear();
}

void call_stack::push_call(const std::shared_ptr<graph::node>& call) {
    m_calls.push_back(call);
}

void call_stack::pop_call() {
    m_calls.pop_back();
}

std::shared_ptr<graph::node> call_stack::head() const {
    if (m_calls.empty()) {
        return nullptr;
    } else {
        return *m_calls.rbegin();
    }
}

}

