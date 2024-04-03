#pragma once

#include <vector>
#include <string>

namespace parser {
struct node {
public:
    node(const std::string& t);
    void print() const;
    std::string text;
    std::string link;
    std::vector<node> children;
    
    bool is_range() const;
    bool is_branch() const;
    bool is_optional() const;
    bool is_link() const;
private:
    void print(const int offset) const;
};
}
