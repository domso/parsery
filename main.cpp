#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <unordered_map>
#include <unordered_set>

#include "src/parser/node_generator.h"

#include <optional>
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
    static constexpr auto label = "root";
};

struct leaf {
    typedef int type;
    static constexpr auto label = "leaf";
};

struct nop {
    typedef int type;
    static constexpr auto label = "nop";
};

struct call {
    typedef std::string type;
    static constexpr auto label = "call";
};

struct join {
    typedef std::string type;
    static constexpr auto label = "join";
};

struct character {
    typedef char type;
    static constexpr auto label = "character";
};

struct range {
    typedef std::pair<char, char> type;
    static constexpr auto label = "range";
};

typedef type_union<A, B, C, root, leaf, join, nop, call, character, range> concrete_node_type;
typedef type_union<A, B, C> concrete_edge_type;

typedef graph_node<
    concrete_node_type,
    concrete_edge_type
> concrete_graph_type;




class node_converter {
public:
    std::shared_ptr<concrete_graph_type> convert(const parser::node& node) {
        m_fragment_counter = 0;

        auto [start, end] = node_to_fragment(node);
        remove_nops(start);
        start->untag_nested_children();

        return start;
    }
private:
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

    node_fragment node_to_fragment(const parser::node& node) {
        auto start = std::make_shared<concrete_graph_type>();
        auto end = std::make_shared<concrete_graph_type>();
        std::shared_ptr<concrete_graph_type> current;
    
        if (m_fragment_counter == 0) {
            start->local.cast_to<root>(m_fragment_counter);
            end->local.cast_to<leaf>(m_fragment_counter);
        } else {
            start->local.cast_to<nop>(m_fragment_counter);
            end->local.cast_to<nop>(m_fragment_counter + 1);
        }

        m_fragment_counter += 2;

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
            auto link_node = std::make_shared<concrete_graph_type>();
            auto join_node = std::make_shared<concrete_graph_type>();

            link_node->local.cast_to<call>(node.link);
            join_node->local.cast_to<join>(node.link);

            start->make_edge(link_node);
            link_node->make_edge(join_node);
            join_node->make_edge(end);

            return {start, end};
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

    int m_fragment_counter = 0;
};

class rule_parser {
public:
    void add_rule(const std::string& name, const std::string& rule) {
        auto parse_node = m_generator.generate(rule);
        auto rule_graph = m_converter.convert(parse_node);

        m_nested_rules[name] = rule_graph;

        parse_node.print();

        std::cout << rule_graph->to_string() << std::endl;
    }
    void add_top_rule(const std::string& name, const std::string& rule) {
        auto parse_node = m_generator.generate(rule);
        auto rule_graph = m_converter.convert(parse_node);

        m_nested_rules[name] = rule_graph;
        m_top_rule = rule_graph;

        parse_node.print();
        std::cout << m_top_rule->to_string() << std::endl;
    }
    void parse(const std::string& text) {
        sequencer(text);
    }
private:
    bool accepts(const std::shared_ptr<concrete_graph_type>& node, const char cmp) {
        if (auto c = node->local.get<character>()) {
            return (**c == cmp);
        }
        if (auto c = node->local.get<range>()) {
            return ((**c).first <= cmp && cmp <= (**c).second);
        }

        return false;
    }

    struct parse_stack_item {
        size_t taken_path;
        std::shared_ptr<concrete_graph_type> previous_node;
        std::string::const_iterator text_position;
        std::string::const_iterator max_reached_text_position;

        bool operator==(const parse_stack_item& other) const {
            return 
                taken_path == other.taken_path &&
                previous_node == other.previous_node &&
                text_position == other.text_position &&
                max_reached_text_position == other.max_reached_text_position;
        }
    };

    struct parse_stack {
        std::vector<parse_stack_item> nodes;
        std::vector<std::shared_ptr<concrete_graph_type>> calls;

        void init(const std::string& text, const std::shared_ptr<concrete_graph_type>& root) {
            nodes.push_back({0, root, text.begin(), text.begin()});
        }

        std::shared_ptr<concrete_graph_type> current() {
            std::shared_ptr<concrete_graph_type> current;

            if (!nodes.empty()) {
                auto& item = *nodes.rbegin();
                
                if (item.previous_node != nullptr && item.taken_path < item.previous_node->degree()) {
                    item.previous_node->child(item.taken_path, [&](const std::shared_ptr<concrete_graph_type>& child) {
                        current = child;
                    });
                }
            }

            return current;
        }

        std::shared_ptr<concrete_graph_type> last() {
            std::shared_ptr<concrete_graph_type> current;

            if (!nodes.empty()) {
                auto& item = *nodes.rbegin();
                
                if (item.previous_node != nullptr && item.taken_path < item.previous_node->degree()) {
                    current = item.previous_node;
                }
            }

            return current;
        }


        void branch_to_next() {
            if (!nodes.empty()) {
                auto& item = *nodes.rbegin();
                item.taken_path++;                
            }
        }

        void push_node(const parse_stack_item& item) {
            nodes.push_back(item);
        }
        void pop_node() {
            nodes.pop_back();
        }
        void push_call(const std::shared_ptr<concrete_graph_type>& call) {
            calls.push_back(call);
        }
        void pop_call() {
            calls.pop_back();
        }
        std::shared_ptr<concrete_graph_type> current_call() {
            if (calls.empty()) {
                return nullptr;
            } else {
                return *calls.rbegin();
            }
        }
        bool contains(const parse_stack_item& item) {
            for (auto rit = nodes.rbegin(); rit != nodes.rend(); ++rit) {
                if (item == *rit) {
                    return true;
                }

                if (rit->text_position < item.text_position) {
                    return false;
                }
            }
            
            return false;
        }


    };

    parse_stack m_stack;
    void sequencer(const std::string text) {
        m_stack.init(text, m_top_rule);



        auto it = text.begin();
        auto max_it = it;


        bool is_solution = false;
        while (!is_solution) {
            auto current = m_stack.current();
            max_it = std::max(max_it, it);

            if (current != nullptr) {
                if (current->local.is<root>() || current->local.is<nop>()) {
                    m_stack.push_node({0, current, it, max_it});
                } else if (current->local.is<leaf>()) {
                    auto call = m_stack.current_call();
                    if (call == nullptr) {
                        // current path leads to final leaf node
                        if (it != text.end()) {
                            m_stack.branch_to_next();
                        } else {
                            is_solution = true;
                        }
                    } else {
                        m_stack.push_node({0, current, it, max_it});
                        m_stack.push_node({0, call, it, max_it});

                        m_stack.pop_call();
                    }
                } else if (auto next = current->local.get<call>()) {
                    bool found = m_stack.contains({0, m_nested_rules[**next], it, max_it});

                    if (found) {
                        m_stack.branch_to_next();
                    } else {
                        current->child(0, [&](const std::shared_ptr<concrete_graph_type>& child) {
                            m_stack.push_call(child);
                        });

                        m_stack.push_node({0, current, it, max_it});
                        m_stack.push_node({0, m_nested_rules[**next], it, max_it});
                    }
                } else if (auto next = current->local.get<join>()) {
                    m_stack.push_node({0, current, it, max_it});
                } else if (it < text.end() && accepts(current, *it)) {
                    ++it;
                    m_stack.push_node({0, current, it, max_it});
                } else {
                    m_stack.branch_to_next();
                }

            } else {
                auto node_top = m_stack.last();

                if (node_top == nullptr) {
                    std::cout << "no solutionb" << std::endl;
                    return;
                }

                if (!(node_top->local.is<root>() || node_top->local.is<nop>() || node_top->local.is<leaf>() || node_top->local.is<call>() || node_top->local.is<join>())) {
                    --it;
                }

                if (node_top->local.is<join>()) {
                    m_stack.push_call(node_top);
                }
                if (node_top->local.is<call>()) {
                    m_stack.pop_call();
                }

                m_stack.pop_node();

                m_stack.branch_to_next();
            }
        }

        std::cout << "solution" << std::endl;

/*
        int i = 0;
        int level = 0;
        for (const auto& [index, node] : node_stack) {
            std::shared_ptr<concrete_graph_type> current;
            node->child(index, [&](const std::shared_ptr<concrete_graph_type>& child) {
                current = child;
            });


            if (node->local.is<join>()) {
                level -= 4;
            }
            level = std::max(0, level);
                    std::cout << std::string(level, ' ') << node->local.to_string() << " " << std::endl;
            i++;

            if (node->local.is<call>()) {
                level += 4;
            }
        }
        */
    }

    node_converter m_converter;
    parser::node_generator m_generator;
    std::unordered_map<std::string, std::shared_ptr<concrete_graph_type>> m_nested_rules;
    std::shared_ptr<concrete_graph_type> m_top_rule;
};



int main(int argc, char **argv) {
    rule_parser p;

    p.add_rule("integer", "(0-9)(0-9)*");
    p.add_rule("real", "(0-9)(0-9)*.(0-9)(0-9)*");
    p.add_rule("number", "[integer]|[real]");
    p.add_rule("operator", "+|\\-|\\*|/");

    p.add_top_rule("term", "([number])|(((\\()|())[term] [operator] [term]((\\))|()))");

    p.parse("(5 + (1 + 2) + 1) * 5");
    


    return 0;
}
