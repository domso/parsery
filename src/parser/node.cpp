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
        std::cout << "'" << text << "'" << std::endl;
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

bool parser::node::is_range() const {
    return children.size() == 3 &&
           children[0].children.empty() &&
           (children[0].text.length() == 1 || (children[0].text.length() == 2 && children[0].text.at(0) == '\\')) &&

           children[1].children.empty() &&
           children[1].text.length() == 1 &&
           children[1].text[0] == '-' &&

           children[2].children.empty() &&
           (children[2].text.length() == 1 || (children[2].text.length() == 2 && children[2].text.at(0) == '\\'));
}

bool parser::node::is_branch() const {
    return children.size() == 3 &&
           children[1].children.empty() &&
           children[1].text.length() == 1 &&
           children[1].text[0] == '|';
}

bool parser::node::is_optional() const {
    return children.size() == 2 && children[1].text.length() == 1 && children[1].text.at(0) == '*';
}

bool parser::node::is_link() const {
    return link.length() > 0;
}

