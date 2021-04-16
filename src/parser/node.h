#pragma once

#include <vector>
#include <string>

namespace parser {
struct node {
public:
    node(const std::string& t);
    void print();
    std::string text;
    std::vector<node> children;
private:
    void print(const int offset);
};
}
