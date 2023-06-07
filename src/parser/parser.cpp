#include "parser.h"
#include <assert.h>
#include <chrono>
#include <iostream>

namespace parser {
void parser::add_rule(const std::string& name, const std::string& rule) {    
    m_nodes.push_back(m_generator.generate(rule));
    m_rule_map[name] = m_nodes.size() - 1;
    m_name_map[m_nodes.size() - 1] = name;
}

void parser::add_top_rule(const std::string& name, const std::string& rule) {
    add_rule(name, rule);
    m_top_rules.push_back(name);
}

void parser::init() {
    m_initial_sequence.clear();
    
    for (auto& s : m_top_rules) {
        auto id = m_rule_map.at(s);
        auto tmp = node_initial_expansion(m_nodes[id]);        
        
        for (auto& r : tmp) {
            r.path.insert(r.path.begin(), id);
            r.history.insert(r.history.begin(), {id, true});
            
            m_initial_sequence.push_back(r);
        }        
    }           
}

bool parser::history_element::operator==(const parser::history_element& other) const {
    return 
        rule == other.rule &&
        open == other.open &&
        section == other.section;
}

bool parser::parse_sequence::operator==(const parser::parse_sequence& other) const {
    return path == other.path && history == other.history;
}

parser::solution parser::parse_to_sequence(const std::string& input) 
{
    parse_context context; 
    context.sequence = m_initial_sequence;

    for (size_t i = 0; i < input.length(); i++) {
        if (!check_character(i, input.length(), input.at(i), context)) {
            return context.result;
        }
    }    
    
    for (auto& current : context.sequence) {
        current.history.insert(current.history.begin(), context.partial_history.begin(), context.partial_history.end());

        if (current.path.empty()) {
            context.result.fully_accepted.push_back(current);
        } else {
            context.result.partial_accepted.push_back(current);
        }
    }        
    
    return context.result;
}

bool parser::check_character(const size_t pos, const size_t n, const char c, parse_context& context) const  {
    std::vector<parser::parse_sequence> active;
    std::vector<parser::parse_sequence> final_seq;
    
    for (auto& current : context.sequence) {
        if (sequence_accepts(current, c)) {                
            current.history[current.history.size() - 1].section++;
            append_vector(active, sequence_increment(current));
        }
    }
    
    if (active.empty()) {
        context.result.rejected = context.sequence;
        context.result.rejected_index = pos;
        
        for (auto& seq : context.result.rejected) {
            seq.history.insert(seq.history.begin(), context.partial_history.begin(), context.partial_history.end());
        }
        
        return false;
    }
    
    final_seq.reserve(final_seq.size() + active.size());

    for (auto& current : active) {
        if (current.path.empty() && pos != n - 1) {                
            auto s0 = add_sequences(current, m_initial_sequence);
            final_seq.reserve(final_seq.capacity() + s0.size());

            for (auto& s : s0) {
                if (!contains<parser::parse_sequence>(final_seq, s)) {
                    final_seq.push_back(std::move(s));
                }                    
            }                
        } else {
            if (!contains<parser::parse_sequence>(final_seq, current)) {
                final_seq.push_back(std::move(current));
            }
        }
    }    
    
    bool found = true;
    
    for (int i = 0; i < final_seq.size(); i++) {            
        if (final_seq[0].history.size() < 2 || final_seq[i].history.size() < 2 || !(final_seq[0].history[0] == final_seq[i].history[0])) {
            found = false;
            break;
        }            
    }        
    
    if (found) {
        if (!final_seq.empty()) {
            context.partial_history.push_back(final_seq[0].history[0]);
        }            
        
        for (auto& s : final_seq) {
            s.history.erase(s.history.begin());
        }
    }
    
    context.sequence = std::move(final_seq);
    return true;
}

bool parser::sequence_accepts(const parser::parse_sequence& current, const char c) const {
    assert(!current.path.empty());
    const node* n = &m_nodes[current.path[0]];
    
    for (size_t i = 1; i < current.path.size() - 1; i++) {        
        if (n->children.empty()) {
            auto name = n->text.substr(1, n->text.length() - 2);
            n = &m_nodes[m_rule_map.at(name)];
        }
                
        n = &(n->children[current.path[i]]);
    }
   
    assert(!current.path.empty()) ;
    if (
        n->is_range() &&
        current.path[current.path.size() - 1] == 1
    ) {        
        return n->children[0].text[0] <= c && c <= n->children[2].text[0];    
    } else {        
        assert(n->children.empty());        
        
        return 
            current.path[current.path.size() - 1] < n->text.length() && 
            n->text.at(current.path[current.path.size() - 1]) == c;        
    }
}

std::vector<parser::parse_sequence> parser::sequence_increment(const parser::parse_sequence& current) const {
    assert(!current.path.empty());
    const node* n = &m_nodes[current.path[0]];
    
    std::vector<const node*> node_stack(current.path.size() - 2);
    std::vector<size_t> index_stack(current.path.size() - 2);
    std::vector<std::pair<size_t, bool>> name_stack(current.path.size() - 2);

    for (size_t i = 1; i < current.path.size() - 1; i++) {        
        if (n->children.empty()) {
            auto name = n->text.substr(1, n->text.length() - 2);
            n = &m_nodes[m_rule_map.at(name)];
            name_stack[i - 1] = {m_rule_map.at(name), true};
        } else {
            name_stack[i - 1] = {0, false};
        }
        
        node_stack[i - 1] = n; 
        index_stack[i - 1] = current.path[i];
        
        n = &(n->children[current.path[i]]);
    }
    
    if (!(
        n->is_range() &&
        current.path[current.path.size() - 1] == 1
    )) {        
        if (current.path[current.path.size() - 1] + 1 < n->text.length()) {
            auto result = current;
            result.path[result.path.size() - 1]++;
            return {result};
        }      
    }
    
    parser::parse_sequence next = current;
    next.path.pop_back();   

    std::vector<parser::parse_sequence> saved_sequences;
    
    saved_sequences.reserve(128);

    for (int i = node_stack.size() - 1; i >= 0; i--) {
        int offset = 1;
        while (index_stack[i] + offset < node_stack[i]->children.size()) {            
            if (
                node_stack[i]->children[index_stack[i] + offset].is_or()
            ) {
                break;
            } else if (
                node_stack[i]->children[index_stack[i] + offset].is_star()
            ) { 
                auto s0 = node_initial_expansion(node_stack[i]->children[index_stack[i]]);
                append_vector(saved_sequences, add_sequences(next, s0));    

                break;
            } else {                
                next.path[next.path.size() - 1]++;
                auto s0 = node_initial_expansion(node_stack[i]->children[index_stack[i] + offset]);
                append_vector(saved_sequences, add_sequences(next, s0));    

                if (
                    node_stack[i]->children[index_stack[i] + offset].is_optional()
                ) {
                    offset++;
                } else {
                    return saved_sequences;
                }                
            }
        }
        
        if (name_stack[i].second) {
            next.history.push_back({name_stack[i].first, false});
        }
        
        next.path.pop_back();
    }
    
    next.path.clear();
    next.history.push_back({current.path[0], false});
    saved_sequences.push_back(next);
    
    return saved_sequences;
}

std::vector<parser::parse_sequence> parser::node_initial_expansion(const node& n) const {       
    if (n.children.empty()) {
        if (n.is_link()) {
            auto id = m_rule_map.at(n.link);
            auto s0 = node_initial_expansion(m_nodes[id]);
            
            for (auto& s : s0) {
                s.history.insert(s.history.begin(), {id, true});
            }
            
            return s0;
        }
        
        return {{{0}, {}}};
    } else {
        if (n.is_range()) {
            return {{{1}, {}}};
        }
            
        if (n.is_branch()) {            
            auto s0 = node_initial_expansion(n.children[0]);
            auto s1 = node_initial_expansion(n.children[2]);
                            
            return merge_sequences(s0, s1, 0, 2);
        }        
        
        std::vector<parser::parse_sequence> result;    
    
        for (size_t i = 0; n.children.size() - 1; i++) {
            auto s0 = node_initial_expansion(n.children[i]);                     
            auto m0 = merge_sequences(s0, {}, i, 0);   

            for (auto& m : m0) {
                result.push_back(m);
            }
                
            if (!n.children[i].is_optional()) {
                break;
            }            
        }   
        
        return result;
    }
}

std::vector<parser::parse_sequence> parser::merge_sequences(const std::vector<parse_sequence>& s0, const std::vector<parse_sequence>& s1, const size_t n0, const size_t n1) const {
    std::vector<parser::parse_sequence> result(s0.size() + s1.size());                
    auto i = 0;

    for (auto& s : s0) {
        parser::parse_sequence seq;
        seq.path.push_back(n0);
        append_vector(seq.path, s.path);
        seq.history = s.history;
        
        result[i] = std::move(seq);
        i++;
    }
    
    for (auto& s : s1) {
        parser::parse_sequence seq;
        seq.path.push_back(n1);
        append_vector(seq.path, s.path);
        seq.history = s.history;
        
        result[i] = std::move(seq);
        i++;
    }
    
    return result;
}

std::vector<parser::parse_sequence> parser::add_sequences(parser::parse_sequence& current, const std::vector<parse_sequence>& s0) const
{
    std::vector<parser::parse_sequence> result(s0.size());
    size_t i = 0;

    for (auto& s : s0) {
        result[i] = current;
        append_vector(result[i].history, s.history);
        append_vector(result[i].path, s.path);

        i++;
    }    

    return result;
}

void parser::print(const std::vector<parse_sequence>& seqvec, const std::string input) const {
    int level = 0;
    for (auto& seq : seqvec) {
        size_t current = 0;
        level = 0;
        std::cout << "### Sequence ###" << std::endl;
        std::cout << "# History := " << std::endl;

        for (auto& h : seq.history) {            
            if (h.open) {
                for (int i = 0; i < level; i++) {
                    std::cout << "    ";
                }
                std::cout << "<" << m_name_map.at(h.rule) << ">" << std::endl;
                level++;
            } else {
                for (int i = 0; i < level - 1; i++) {
                    std::cout << "    ";
                }
                std::cout << "</" << m_name_map.at(h.rule) << ">" << std::endl;                
                level--;
            }
            if (h.section > 0) {            
                for (int i = 0; i < level; i++) {
                    std::cout << "    ";
                }
                std::cout << "'" << input.substr(current, h.section) << "'" << std::endl;
                current += h.section;
            }
        }
        
//         std::cout << std::endl;
        std::cout << "# Path := ";
        for (auto& p : seq.path) {
            std::cout << p << " ";
        }
        std::cout << std::endl;
    }
}

parser::parsed_node parser::build_tree(const std::vector<parse_sequence>& sequences, const std::string& input) const {
    parsed_node result;

    for (auto& sequence : sequences) {
        std::vector<parsed_node*> stack;
        stack.push_back(&result);
        size_t current_pos = 0;
        size_t line_num = 0;
        size_t column_num = 0;

        for (auto& h : sequence.history) {            
            if (h.open) {
                parsed_node open_node;
                open_node.name = m_name_map.at(h.rule);
                open_node.line = line_num;
                open_node.column = column_num;

                stack[stack.size() - 1]->children.push_back(open_node);
                stack.push_back(&(*stack[stack.size() - 1]->children.rbegin()));
            } else {
                stack.pop_back();
            }
            if (h.section > 0) {            
                parsed_node text_node;
                text_node.name = input.substr(current_pos, h.section);
                text_node.line = line_num;
                text_node.column = column_num;

                for (const auto c : text_node.name) {
                    if (c == '\n') {
                        line_num++;
                        column_num = 0;
                    } else {
                        column_num++;
                    }
                }

                stack[stack.size() - 1]->children.push_back(text_node);
                current_pos += h.section;
            }
        }
    }

    return result;
}
}
