#include "node.h"

#include <iostream>

parser::node::node(const std::string& t) : text(t) {

}

void parser::node::print() const
{
    print(0);
}

void parser::node::print(const int offset) const
{
    for (int i = 0; i < offset; i++) {
        std::cout << "    ";
    }
    if (children.empty()) {
        std::cout << text << std::endl;
    } else {
        std::cout << text << "(" << std::endl;
        for (auto& c : children) {
            c.print(offset + 1);
        }
        for (int i = 0; i < offset; i++) {
            std::cout << "    ";
        }
        std::cout << ")" << std::endl;
    }
}

