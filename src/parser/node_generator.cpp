#include "node_generator.h"

parser::node parser::node_generator::generate(const std::string& text) const
{
    auto extended_text = extend_or(extend_star(extend_range(extend_label(text, 0), 0), 0), 0);

    node result(extended_text);

    split_node(result);
    reduce_node(result);
    clean_node(result);    
    
    return result;
}

std::string parser::node_generator::extend_star(const std::string& text, const int num) const
{
    bool escaped = false;
    int found = 0;
    int brackets = 0;
    int src_brackets = 0;

    for (size_t i = 0; i < text.length(); i++) {
        if (escaped) {
            escaped = false;
        } else {
            if (text.at(i) == '(') {
                src_brackets++;
            }
            if (text.at(i) == ')') {
                src_brackets--;
            }

            if (text.at(i) == '*') {
                if (found == num) {
                    escaped = false;
                    int max = 0;
                    for (size_t j = 0; j < i; j++) {
                        if (escaped) {
                            escaped = false;
                        } else {
                            if (brackets == src_brackets) {
                                max = j;
                            }
                            if (text.at(j) == '(') {
                                brackets++;
                            }
                            if (text.at(j) == ')') {
                                brackets--;
                            }
                        }

                        escaped = (text.at(j) == '\\');
                    }

                    if (i < text.length() - 1) {
                        return extend_star(text.substr(0, max) + "((" + text.substr(max, i - max) + ")*)" + text.substr(i + 1), num + 1);
                    } else {
                        return extend_star(text.substr(0, max) + "((" + text.substr(max, i - max) + ")*)", num + 1);
                    }
                } else {
                    found++;
                }
            }

            escaped = (text.at(i) == '\\');
        }
    }

    return text;
}

std::string parser::node_generator::extend_or(const std::string& text, const int num) const
{
    bool escaped = false;
    int found = 0;
    int brackets = 0;
    int src_brackets = 0;
    size_t start;

    for (size_t i = 0; i < text.length(); i++) {
        if (escaped) {
            escaped = false;
        } else {
            if (text.at(i) == '(') {
                src_brackets++;
            }
            if (text.at(i) == ')') {
                src_brackets--;
            }

            if (text.at(i) == '|') {
                if (found == num) {

                    start = 0;

                    escaped = false;
                    start = 0;

                    for (size_t j = 0; j < i; j++) {
                        if (escaped) {
                            escaped = false;
                        } else {
                            if (brackets == src_brackets) {
                                start = j;
                            }
                            if (text.at(j) == '(') {
                                brackets++;
                            }
                            if (text.at(j) == ')') {
                                brackets--;
                            }
                        }

                        escaped = (text.at(j) == '\\');
                    }

                    brackets = 0;

                    escaped = false;
                    for (size_t j = i + 1; j <= text.length(); j++) {
                        if (!escaped) {
                            if (text.at(j) == '(') {
                                brackets++;
                            }
                            if (text.at(j) == ')') {
                                brackets--;
                            }
                        }
                        
                        if (brackets == 0 && (escaped || text.at(j) != '\\')) {
                            if (j < text.length() - 1) {
                                return extend_or(
                                            text.substr(0, start) +
                                            "((" +
                                            text.substr(start, i - start) +
                                            ")|(" +
                                            text.substr(i + 1, j - (i + 1) + 1) +
                                            "))" +
                                            text.substr(j + 1)
                                            , num + 1
                                        );
                            } else {
                                return extend_or(
                                            text.substr(0, start) +
                                            "((" +
                                            text.substr(start, i - start) +
                                            ")|(" +
                                            text.substr(i + 1, j - (i + 1) + 1) +
                                            "))"
                                            , num + 1
                                        );
                            }
                        }                        
                
                        if (escaped) {
                            escaped = (text.at(j) == '\\');
                        } else {
                            escaped = false;
                        }                        
                    }
                } else {
                    found++;
                }
            }

            escaped = (text.at(i) == '\\');
        }
    }

    return text;
}

std::string parser::node_generator::extend_range(const std::string& text, const int num) const
{
    bool escaped = false;
    int found = 0;
    int brackets = 0;
    int src_brackets = 0;
    size_t start;

    for (size_t i = 0; i < text.length(); i++) {
        if (escaped) {
            escaped = false;
        } else {
            if (text.at(i) == '(') {
                src_brackets++;
            }
            if (text.at(i) == ')') {
                src_brackets--;
            }

            if (text.at(i) == '-') {
                if (found == num) {

                    start = 0;

                    escaped = false;
                    start = 0;

                    for (size_t j = 0; j < i; j++) {
                        if (escaped) {
                            escaped = false;
                        } else {
                            if (brackets == src_brackets) {
                                start = j;
                            }
                            if (text.at(j) == '(') {
                                brackets++;
                            }
                            if (text.at(j) == ')') {
                                brackets--;
                            }
                        }

                        escaped = (text.at(j) == '\\');
                    }

                    brackets = 0;

                    escaped = false;
                    for (size_t j = i + 1; j <= text.length(); j++) {
                        if (!escaped) {
                            if (text.at(j) == '(') {
                                brackets++;
                            }
                            if (text.at(j) == ')') {
                                brackets--;
                            }
                        }
                        if (brackets == 0 && (escaped || text.at(j) != '\\')) {
                            if (j < text.length() - 1) {
                                return extend_range(
                                            text.substr(0, start) +
                                            "((" +
                                            text.substr(start, i - start) +
                                            ")-(" +
                                            text.substr(i + 1, j - (i + 1) + 1) +
                                            "))" +
                                            text.substr(j + 1)
                                            , num + 1
                                        );
                            } else {
                                return extend_range(
                                            text.substr(0, start) +
                                            "((" +
                                            text.substr(start, i - start) +
                                            ")-(" +
                                            text.substr(i + 1, j - (i + 1) + 1) +
                                            "))"
                                            , num + 1
                                        );
                            }
                        }                                              
                
                        if (escaped) {
                            escaped = (text.at(j) == '\\');
                        } else {
                            escaped = false;
                        }   
                    }
                } else {
                    found++;
                }
            }

            escaped = (text.at(i) == '\\');
        }
    }

    return text;
}

std::string parser::node_generator::extend_label(const std::string& text, const int num) const
{
    bool escaped = false;
    int found = 0;
    
    for (size_t i = 0; i < text.length(); i++) {
        if (escaped) {
            escaped = false;
        } else {
            if (text.at(i) == '[') {
                if (found == num) {
                    return extend_label(text.substr(0, i) + "(" + text.substr(i), num + 1);
                } else {
                    found++;
                }
            }
            if (text.at(i) == ']') {
                if (found == num) {
                    return extend_label(text.substr(0, i + 1) + ")" + text.substr(i + 1), num + 1);
                } else {
                    found++;
                }
            }

            escaped = (text.at(i) == '\\');
        }
    }

    return text;
}

void parser::node_generator::reduce_node(parser::node& n) const
{
    for (auto& c : n.children) {
        reduce_node(c);
    }

    if (n.children.size() == 1) {
        n.text = n.children[0].text;
        n.children = n.children[0].children;
    }
}

void parser::node_generator::split_node(parser::node& n) const
{
    split_block_node(n);

    for (auto& c : n.children) {
        split_node(c);
    }
}

void parser::node_generator::split_block_node(parser::node& n) const
{
    std::vector<std::pair<size_t, size_t>> blocks;

    bool escaped = false;
    int found = false;
    size_t start;

    for (size_t i = 0; i < n.text.length(); i++) {
        if (escaped) {
            escaped = false;
        } else {
            if (found == 0) {
                if (n.text.at(i) == '(') {
                    start = i;
                    found = 1;
                }
            } else {
                if (n.text.at(i) == '(') {
                    found++;
                }
                if (n.text.at(i) == ')') {
                    found--;

                    if (found == 0) {
                        blocks.push_back({start, i});
                    }
                }
            }

            escaped = (n.text.at(i) == '\\');
        }
    }

    if (blocks.size() > 0) {
        size_t current = 0;

        for (auto& l : blocks) {
            auto head = n.text.substr(current, l.first - current);

            if (head != "") {
                n.children.push_back({head});
            }

            n.children.push_back({n.text.substr(l.first + 1, l.second - l.first - 1)});
            current = l.second + 1;
        }

        if (current < n.text.length()) {
            n.children.push_back({n.text.substr(current)});
        }

        n.text = "";
    }
}

void parser::node_generator::clean_node(parser::node& n) const {
    std::string result;
    
    bool escaped = false;
    for (auto c : n.text) {
        if (escaped) {
            result += c;
        } else {
            if (c == '\\') {
                escaped = true;
            } else {
                result += c;
            }
        }
    }    
    
    n.text = result;    
    
    for (auto& c : n.children) {
        clean_node(c);
    }
}




