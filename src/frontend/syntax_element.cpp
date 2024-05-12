#include "syntax_element.h"

#include <iostream>

namespace parser::frontend {

syntax_element::syntax_element(const std::string& t) : text(t) {

}

bool syntax_element::is_range() const {
    return children.size() == 3 &&
           children[0].children.empty() &&
           (children[0].text.length() == 1 || (children[0].text.length() == 2 && children[0].text.at(0) == '\\')) &&

           children[1].children.empty() &&
           children[1].text.length() == 1 &&
           children[1].text[0] == '-' &&

           children[2].children.empty() &&
           (children[2].text.length() == 1 || (children[2].text.length() == 2 && children[2].text.at(0) == '\\'));
}

bool syntax_element::is_branch() const {
    return children.size() == 3 &&
           children[1].children.empty() &&
           children[1].text.length() == 1 &&
           children[1].text[0] == '|';
}

bool syntax_element::is_optional() const {
    return children.size() == 2 && children[1].text.length() == 1 && children[1].text.at(0) == '*';
}

bool syntax_element::is_link() const {
    return link.length() > 0;
}

void syntax_element::print() const {
    print(0);
}

void syntax_element::print(const int offset) const {
    for (int i = 0; i < offset; i++) {
        std::cout << "    ";
    }

    if (children.empty()) {
        std::cout << "'" << text << "---" << link << "'" << std::endl;
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

}
