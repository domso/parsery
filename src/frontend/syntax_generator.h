#pragma once

#include <vector>
#include <string>

#include "syntax_element.h"

namespace parsery::frontend {

class syntax_generator {
public:
    syntax_element generate(const std::string& text) const;
private:
    std::string extend_star(const std::string& text, const int num) const;
    std::string extend_or(const std::string& text, const int num) const;
    std::string extend_range(const std::string& text, const int num) const;
    std::string extend_label(const std::string& text, const int num) const;
    
    void reduce_syntax_element(syntax_element& n) const;
    void split_syntax_element(syntax_element& n) const;
    void split_block_syntax_element(syntax_element& n) const;
    void link_syntax_element(syntax_element& n) const;
    void clean_syntax_element(syntax_element& n) const;

    template<typename T>
    void iterate_over_escaped_string(const std::string& text, const size_t start, const size_t end, T call) const {
        bool escaped = false;
        for (size_t i = start; i < end; i++) {
            if (escaped) {
                escaped = false;
            } else {
                if (!call(i)) {
                    return;
                }
                escaped = (text.at(i) == '\\');
            }
        }
    }
};

}
