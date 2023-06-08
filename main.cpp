#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <unordered_map>
#include <unordered_set>

#include "src/parser/node_generator.h"
#include "src/parser/parser.h"

#include "src/parser/type_union.h"
#include "src/parser/graph_node.h"
#include <memory>


struct A {
    typedef int type;
    static constexpr auto label = "A";
};
struct B {
    typedef float type;
    static constexpr auto label = "B";
};
struct C {
    typedef std::string type;
    static constexpr auto label = "C";
};

struct root {
    typedef int type;
    static constexpr auto label = "Root";
};

struct nop {
    typedef int type;
    static constexpr auto label = "nop";
};

struct character {
    typedef char type;
    static constexpr auto label = "character";
};

struct range {
    typedef std::pair<char, char> type;
    static constexpr auto label = "range";
};

typedef type_union<A, B, C, root, nop, character, range> concrete_node_type;
typedef type_union<A, B, C> concrete_edge_type;

typedef graph_node<
    concrete_node_type,
    concrete_edge_type
> concrete_graph_type;


bool accepts(const std::shared_ptr<concrete_graph_type>& node, const char cmp) {
    if (auto c = node->local.get<character>()) {
        return (**c == cmp);
    }
    if (auto c = node->local.get<range>()) {
        return ((**c).first <= cmp && cmp <= (**c).second);
    }

    return false;
}

void sequencer(std::shared_ptr<concrete_graph_type>& start, const std::string text) {
    std::vector<std::pair<size_t, std::shared_ptr<concrete_graph_type>>> node_stack;

    node_stack.push_back({0, start});

    auto it = text.begin();

    while (it != text.end()) {
        auto& [index_top, node_top] = *node_stack.rbegin();

        if (index_top < node_top->degree()) {
            std::shared_ptr<concrete_graph_type> current;
            node_top->child(index_top, [&](const std::shared_ptr<concrete_graph_type>& child) {
                current = child;
            });

        std::cout << *it << " " << current->local.to_string() << std::endl;

            if (current->local.is<root>() || current->local.is<nop>()) {
                node_stack.push_back({0, current});
            } else if (accepts(current, *it)) {
                ++it;
                node_stack.push_back({0, current});
            } else {
                index_top++;
            }
        } else {
            if (!(node_top->local.is<root>() || node_top->local.is<nop>())) {
                --it;
            }

            node_stack.pop_back();
            if (node_stack.empty()) {
                std::cout << "no solution" << std::endl;
                return;
            } else {
                node_stack.rbegin()->first++;
            }
        }
    }

    for (const auto& [index, node] : node_stack) {
        std::cout << index << " " << node->local.to_string() << std::endl;
    }
}


class node_converter {
public:
    typedef std::pair<std::shared_ptr<concrete_graph_type>, std::shared_ptr<concrete_graph_type>> node_fragment;


    void remove_nops(const std::shared_ptr<concrete_graph_type>& root) {
        if (!root->tag()) {
            auto current = root;

            std::vector<std::shared_ptr<concrete_graph_type>> remove_childs;

            current->tag() = true;
            current->for_each_child([&](std::shared_ptr<concrete_graph_type>& child) {
                remove_nops(child);
            });

            current->for_each_child([&](std::shared_ptr<concrete_graph_type>& child) {
                if (child->local.is<nop>()) {
                    remove_childs.push_back(child);
                    child->for_each_child([&](std::shared_ptr<concrete_graph_type>& grandchild) {
                        current->make_edge(grandchild);
                    });
                }
            });

            for (const auto i : remove_childs) {
                current->remove_edge(i);
            }
        }
    }

    int fragment_counter = 0;
    node_fragment node_to_fragment(const parser::node& node) {
        auto start = std::make_shared<concrete_graph_type>();
        auto end = std::make_shared<concrete_graph_type>();
        std::shared_ptr<concrete_graph_type> current;
    
        if (fragment_counter == 0) {
            start->local.cast_to<root>(fragment_counter);
            end = start;
        } else {
            start->local.cast_to<nop>(fragment_counter);
            end->local.cast_to<nop>(fragment_counter + 1);
        }

        fragment_counter += 2;

        if (node.is_range()) {
            auto range_node = std::make_shared<concrete_graph_type>();
            range_node->local.cast_to<range>({node.children[0].text[0], node.children[2].text[0]});
            start->make_edge(range_node);
            range_node->make_edge(end);

            return {start, end};
        }
        if (node.is_branch()) {
            auto [left_start, left_end] = node_to_fragment(node.children[0]);
            auto [right_start, right_end] = node_to_fragment(node.children[2]);

            start->make_edge(left_start);
            start->make_edge(right_start);

            left_end->make_edge(end);
            right_end->make_edge(end);

            return {start, end};

        }
        if (node.is_optional()) {
            auto [left_start, left_end] = node_to_fragment(node.children[0]);

            start->make_edge(left_start);
            start->make_edge(end);
            left_end->make_edge(left_start);
            left_end->make_edge(end);

            return {start, end};
        }
        if (node.is_link()) {

        }
        if (node.text.length() == 0) {
            current = start;

            for (const auto& child : node.children) {
                auto [child_start, child_end] = node_to_fragment(child);

                current->make_edge(child_start);
                current = child_end;
            }

            current->make_edge(end);

            return {start, end};
        }

        current = start;
        for (auto c : node.text) {
            current->make_edge([&](
                std::shared_ptr<concrete_graph_type>& target
            ) {
                target->local.cast_to<character>(c);
                
                current = target;
            });
        }
        current->make_edge(end);

        return {start, end};
    }





private:
};



int main(int argc, char **argv) {
    parser::node_generator p;
    
{
    auto n = p.generate("(hal*o)(a-z|A-Z)(((a-z)!)|A-Z|0-9)*");
    //auto n = p.generate("o*");
    n.print();

    node_converter conv;
    auto [start, end] = conv.node_to_fragment(n);

    //std::cout << start->to_string() << std::endl;
    conv.remove_nops(start);
    start->untag_nested_children();
    std::cout << start->to_string() << std::endl;

    sequencer(start, "haloaa!");
}


    return 0;
}
