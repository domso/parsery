#include "parser.h"
#include <assert.h>
#include <chrono>
#include <iostream>

    int measured_times[4] = {0, 0, 0, 0};
void parser::parser::add_rule(const std::string& name, const std::string& rule) {    
    m_nodes.push_back(m_generator.generate(rule));
    m_rule_map[name] = m_nodes.size() - 1;
    m_name_map[m_nodes.size() - 1] = name;
}

void parser::parser::add_top_rule (const std::string& name, const std::string& rule) {
    add_rule(name, rule);
    m_top_rules.push_back(name);
}

void parser::parser::init() {
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

bool parser::parser::history_element::operator==(const parser::parser::history_element& other) const {
    return 
        rule == other.rule &&
        open == other.open &&
        section == other.section;
}

bool parser::parser::parse_sequence::operator==(const parser::parser::parse_sequence& other) const {
    return path == other.path && history == other.history && parent == other.parent;
}

parser::parser::solution parser::parser::parse_to_sequence(const std::string& input) 
{
    parser::parser::solution result;
    std::vector<parser::parser::parse_sequence> sequence = m_initial_sequence;
    std::vector<history_element> partial_history;
    
            auto t0 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < input.length(); i++) {
        std::vector<parser::parser::parse_sequence> cleared;
        std::vector<parser::parser::parse_sequence> active;
        std::vector<parser::parser::parse_sequence> final_seq;
        
            auto t1 = std::chrono::steady_clock::now();
        for (auto& current : sequence) {
            if (sequence_accepts(current, input.at(i))) {                
                current.history[current.history.size() - 1].section++;

                if (current.parent) {
                    std::vector<size_t> tmp(current.path.size() + m_parents.at(*current.parent).size());
                    auto p = *current.parent;
                    for (size_t i = 0; i < m_parents.at(p).size(); i++) {
                        tmp[i] = m_parents.at(p)[i];
                    }
                    for (size_t i = 0; i < current.path.size(); i++) {
                        tmp[m_parents.at(p).size() + i] = current.path[i];
                    }

                    current.parent = std::nullopt;
                    current.path = std::move(tmp);
                }
            
                cleared.push_back(std::move(current));
            }
        }
        m_parent_id = 0;
        m_parents.clear();

        for (auto& current : cleared) {
            auto next = sequence_increment(current);
            size_t j = active.size();
            active.resize(active.size() + next.size());

            for (auto& s : next) {
                active[j] = std::move(s);                    
                j++;
            }
        }
        
                auto t2 = std::chrono::steady_clock::now();
        if (active.empty()) {
            result.rejected = sequence;
            result.rejected_index = i;
            
            for (auto& seq : result.rejected) {
                seq.history.insert(seq.history.begin(), partial_history.begin(), partial_history.end());
            }
            
            return result;
        }
        
        auto t3 = std::chrono::steady_clock::now();
        final_seq.reserve(final_seq.size() + active.size());

        for (auto& current : active) {
            if (current.path.empty() && (!current.parent) && i != input.length() - 1) {                
                auto s0 = add_sequences(current, m_initial_sequence);
                final_seq.reserve(final_seq.capacity() + s0.size());

                for (auto& s : s0) {
                    if (!contains<parser::parser::parse_sequence>(final_seq, s)) {
                        final_seq.push_back(std::move(s));
                    }                    
                }                
            } else {
                if (!contains<parser::parser::parse_sequence>(final_seq, current)) {
                    final_seq.push_back(std::move(current));
                }
            }
        }    
        
        auto t4 = std::chrono::steady_clock::now();
        
                measured_times[0] += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
                measured_times[1] += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
                measured_times[2] += std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
        
        bool found = true;
        
        for (int i = 0; i < final_seq.size(); i++) {            
            if (final_seq[0].history.size() < 2 || final_seq[i].history.size() < 2 || !(final_seq[0].history[0] == final_seq[i].history[0])) {
                found = false;
                break;
            }            
        }        
        
        
        if (found) {
            if (!final_seq.empty()) {
                partial_history.push_back(final_seq[0].history[0]);
            }            
            
            for (auto& s : final_seq) {
                s.history.erase(s.history.begin());
            }
        }
        
        sequence = final_seq;
        sequence = std::move(final_seq);
    }    
            auto t6 = std::chrono::steady_clock::now();

                measured_times[3] += std::chrono::duration_cast<std::chrono::microseconds>(t6 - t0).count();
    

    std::cout << measured_times[0] << std::endl;
    std::cout << measured_times[1] << std::endl;
    std::cout << measured_times[2] << std::endl;
    std::cout << measured_times[3] << std::endl;
    
    for (auto& current : sequence) {
        if (current.path.empty() && (!current.parent)) {
            current.history.insert(current.history.begin(), partial_history.begin(), partial_history.end());
            result.fully_accepted.push_back(current);
        } else {
            result.partial_accepted.push_back(current);
        }
    }        
    
    return result;
}

bool parser::parser::sequence_accepts(const parser::parser::parse_sequence& current, const char c) const {
    assert(!current.path.empty());
    const node* n;

    int normal_path_start = 1;
    if (current.parent) {
        auto& p = *current.parent;
        assert(m_parents.at(p).size() > 0);
        n = &m_nodes[m_parents.at(p)[0]];
        
        for (size_t i = 1; i < m_parents.at(p).size() - 1; i++) {        
            if (n->children.empty()) {
                auto name = n->text.substr(1, n->text.length() - 2);
                n = &m_nodes[m_rule_map.at(name)];
            }
                    
            n = &(n->children[m_parents.at(p)[i]]);
        }
        normal_path_start = 0;
    } else {
        n = &m_nodes[current.path[0]];
    }
    
    for (size_t i = normal_path_start; i < current.path.size() - 1; i++) {        
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

std::vector<parser::parser::parse_sequence> parser::parser::sequence_increment(const parser::parser::parse_sequence& current) {
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
    
    parser::parser::parse_sequence next = current;
    next.path.pop_back();   

    std::vector<parser::parser::parse_sequence> saved_sequences;
    
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
            auto t1 = std::chrono::steady_clock::now();
                auto s0 = node_initial_expansion(node_stack[i]->children[index_stack[i]]);
            auto t2 = std::chrono::steady_clock::now();
                auto seq = add_sequences(next, s0);    
            auto t3 = std::chrono::steady_clock::now();
                size_t j = saved_sequences.size();
                saved_sequences.resize(saved_sequences.size() + seq.size());
                for (auto& s : seq) {
                    saved_sequences[j++] = std::move(s);
                }
                break;
            } else {                
            auto t1 = std::chrono::steady_clock::now();
                next.path[next.path.size() - 1]++;
                auto s0 = node_initial_expansion(node_stack[i]->children[index_stack[i] + offset]);
            auto t2 = std::chrono::steady_clock::now();
                auto seq = add_sequences(next, s0);    
            auto t3 = std::chrono::steady_clock::now();
                size_t j = saved_sequences.size();
                saved_sequences.resize(saved_sequences.size() + seq.size());
                for (auto& s : seq) {
                    saved_sequences[j++] = std::move(s);
                }
            auto t4 = std::chrono::steady_clock::now();

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

std::vector<parser::parser::parse_sequence> parser::parser::node_initial_expansion(const node& n) const {       
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
        
        std::vector<parser::parser::parse_sequence> result;    
    
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

std::vector<parser::parser::parse_sequence> parser::parser::merge_sequences(const std::vector<parse_sequence>& s0, const std::vector<parse_sequence>& s1, const size_t n0, const size_t n1) const {
    std::vector<parser::parser::parse_sequence> result;                
    
    for (auto& s : s0) {
        parser::parser::parse_sequence seq;
        seq.path.push_back(n0);
        for (auto& p : s.path) {
            seq.path.push_back(p);
        }
        seq.history = s.history;
        
        result.push_back(seq);
    }
    
    for (auto& s : s1) {
        parser::parser::parse_sequence seq;
        seq.path.push_back(n1);
        for (auto& p : s.path) {
            seq.path.push_back(p);
        }
        seq.history = s.history;
        
        result.push_back(seq);
    }
    
    return result;
}

std::vector<parser::parser::parse_sequence> parser::parser::add_sequences(parser::parser::parse_sequence& current, const std::vector<parse_sequence>& s0)
{
            auto t1 = std::chrono::steady_clock::now();
    std::vector<parser::parser::parse_sequence> result(s0.size());
    size_t i = 0;

    if (current.parent ) {
        assert(false);
        std::vector<size_t> tmp(current.path.size() + m_parents.at(*current.parent).size());
        auto p = *current.parent;
        for (size_t i = 0; i < m_parents.at(p).size(); i++) {
            tmp[i] = m_parents.at(p)[i];
        }
        for (size_t i = 0; i < current.path.size(); i++) {
            tmp[m_parents.at(p).size() + i] = current.path[i];
        }

        current.parent = std::nullopt;
        current.path = std::move(tmp);
    }

    if (!current.path.empty()) {
        m_parents[m_parent_id] = current.path;
    }

    for (auto& s : s0) {

/*
        auto j = result[i].history.size();
        result[i].history.resize(j + s.history.size());

        for (auto& h : s.history) {
            result[i].history[j] = h;
            j++;
        }
  */    

            result[i] = s;
        if (current.path.empty()) {
        } else {
            result[i].parent = m_parent_id;
        }
        
        i++;
    }    
    if (!current.path.empty()) {
        m_parent_id++;
    }

    return result;
}

void parser::parser::print(const std::vector<parse_sequence>& seqvec, const std::string input) const {
    int level = 0;
    size_t current = 0;
    for (auto& seq : seqvec) {
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
