#pragma once

#include <vector>
#include <string>

namespace parser::frontend {

struct syntax_element {
    syntax_element(const std::string& t);
    std::string text;
    std::string link;
    std::vector<syntax_element> children;
    
    bool is_range() const;
    bool is_branch() const;
    bool is_optional() const;
    bool is_link() const;

    void print() const;
private:
    void print(const int offset) const;
};

}
