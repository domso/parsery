#include "node_generator.h"
#include <iostream>

template<typename T>
void iterate_over_escaped_string(const std::string& text, const size_t start, const size_t end, T call) {
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

parser::node parser::node_generator::generate(const std::string& text) const
{    
    auto with_label = extend_label(text, 0);
    auto with_range = extend_range(with_label, 0);
    auto with_star = extend_star(with_range, 0);
    auto with_or = extend_or(with_star, 0);
    auto extended_text = with_or;

    std::cout
    << text
    << "\n" << with_label
    << "\n" << with_range
    << "\n" << with_star
    << "\n" << with_or
    << "\n" << extended_text << std::endl;

    node result(extended_text);

    result.print();

    split_node(result);
    result.print();
    reduce_node(result);
    result.print();
    link_node(result);
    result.print();
    
    return result;
}

std::string parser::node_generator::extend_star(const std::string& text, const int num) const
{
    std::string result = text;
    int found = 0;
    int brackets = 0;
    int src_brackets = 0;

    iterate_over_escaped_string(text, 0, text.length(), [&](const size_t i) {
        if (text.at(i) == '(') {
            src_brackets++;
        }
        if (text.at(i) == ')') {
            src_brackets--;
        }

        if (text.at(i) == '*') {
            if (found == num) {
                int max = 0;
                iterate_over_escaped_string(text, 0, i, [&](const size_t j) {
                    if (brackets == src_brackets) {
                        max = j;
                    }
                    if (text.at(j) == '(') {
                        brackets++;
                    }
                    if (text.at(j) == ')') {
                        brackets--;
                    }
                    return true;
                });

                if (i < text.length() - 1) {
                    result = extend_star(text.substr(0, max) + "((" + text.substr(max, i - max) + ")*)" + text.substr(i + 1), num + 1);
                    return false;
                } else {
                    result = extend_star(text.substr(0, max) + "((" + text.substr(max, i - max) + ")*)", num + 1);
                    return false;
                }
            } else {
                found++;
            }
        }

        return true;
    });

    return result;
}

std::string parser::node_generator::extend_or(const std::string& text, const int num) const
{
    std::string result = text;
    int found = 0;
    int brackets = 0;
    int src_brackets = 0;
    size_t start;
    size_t end;

    iterate_over_escaped_string(text, 0, text.length(), [&](const size_t i) {
        if (text.at(i) == '(') {
            src_brackets++;
        }
        if (text.at(i) == ')') {
            src_brackets--;
        }

        if (text.at(i) == '|') {
            if (found == num) {

                start = 0;
                brackets = 0;

                iterate_over_escaped_string(text, 0, i, [&](const size_t j) {
                    if (brackets == src_brackets) {
                        start = j;
                    }
                    if (text.at(j) == '(') {
                        brackets++;
                    }
                    if (text.at(j) == ')') {
                        brackets--;
                    }
                    return true;
                });

                brackets = 0;

                end = text.length() - 1;
                
                iterate_over_escaped_string(text, i + 1, text.length(), [&](const size_t j) {
                    if (text.at(j) == '(') {
                        brackets++;
                    }
                    
                    if (text.at(j) == ')') {
                        brackets--;
                    }
                    
                    if (brackets == -1 || (brackets == 0 && text.at(j) == '|')) {
                        end = j - 1;
                        return false;
                    }
                    return true;
                });

                if (end < text.length() - 1) {
                    result = extend_or(
                                text.substr(0, start) +
                                "((" +
                                text.substr(start, i - start) +
                                ")|(" +
                                text.substr(i + 1, end - (i + 1) + 1) +
                                "))" +
                                text.substr(end + 1)
                                , num + 1
                            );
                    return false;
                } else {                        
                    result = extend_or(
                                text.substr(0, start) +
                                "((" +
                                text.substr(start, i - start) +
                                ")|(" +
                                text.substr(i + 1, end - (i + 1) + 1) +
                                "))"
                                , num + 1
                            );
                    return false;
                }
            } else {
                found++;
            }
        }
        return true;
    });

    return result;
}

std::string parser::node_generator::extend_range(const std::string& text, const int num) const
{
    std::string result = text;
    int found = 0;
    int brackets = 0;
    int src_brackets = 0;

    iterate_over_escaped_string(text, 0, text.length(), [&](const size_t i) {
        if (text.at(i) == '(') {
            src_brackets++;
        }
        if (text.at(i) == ')') {
            src_brackets--;
        }

        if (text.at(i) == '-') {
            if (found == num) {

                size_t start = 0;

                iterate_over_escaped_string(text, 0, i, [&](const size_t j) {
                    if (brackets == src_brackets) {
                        start = j;
                    }
                    if (text.at(j) == '(') {
                        brackets++;
                    }
                    if (text.at(j) == ')') {
                        brackets--;
                    }
                    return true;
                });

                brackets = 0;
                auto end = i;

                if (i + 1 < text.length()) {
                    if (text.at(i + 1) != '\\') {
                        end = i + 1;
                    } else if (i + 2 < text.length()) {
                        end = i + 2;
                    }
                }

                if (end < text.length() - 1) {
                    result = extend_range(
                        text.substr(0, start) +
                        "((" +
                        text.substr(start, i - start) +
                        ")-(" +
                        text.substr(i + 1, end - i) +
                        "))" +
                        text.substr(end + 1)
                        , num + 1
                    );
                    return false;
                } else {
                    result = extend_range(
                        text.substr(0, start) +
                        "((" +
                        text.substr(start, i - start) +
                        ")-(" +
                        text.substr(i + 1, end - i) +
                        "))"
                        , num + 1
                    );
                    return false;
                }
            } else {
                found++;
            }
        }

        return true;
    });

    return result;
}

std::string parser::node_generator::extend_label(const std::string& text, const int num) const
{
    int found = 0;
    std::string result = text;
    
    iterate_over_escaped_string(text, 0, text.length(), [&](const size_t i) {
        if (text.at(i) == '[') {
            if (found == num) {
                result = extend_label(text.substr(0, i) + "(" + text.substr(i), num + 1);
                return false;
            } else {
                found++;
            }
        }
        if (text.at(i) == ']') {
            if (found == num) {
                result = extend_label(text.substr(0, i + 1) + ")" + text.substr(i + 1), num + 1);
                return false;
            } else {
                found++;
            }
        }
        return true;
    });

    return result;
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

    int found = false;
    size_t start;

    iterate_over_escaped_string(n.text, 0, n.text.length(), [&](const size_t i) {
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
        return true;
    });

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

void parser::node_generator::link_node(parser::node& n) const {   
    
    if (
        n.text.length() > 3 &&
        *n.text.begin() == '[' &&
        *n.text.rbegin() == ']'
    ) {
        n.link = n.text.substr(1, n.text.length() - 2);
    }
    
    for (auto& c : n.children) {
        link_node(c);
    }
}

