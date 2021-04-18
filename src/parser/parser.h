#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "node_generator.h"

namespace parser {
    class parser {
    public:
        void add_rule(const std::string& name, const std::string& rule);
        void add_top_rule(const std::string& name, const std::string& rule);
        
        void init();
//     private:
        struct parse_sequence {
            std::vector<size_t> path;
            std::vector<std::pair<size_t, bool>> history;
        };
        
        std::vector<parse_sequence> parse_to_sequence(const std::string& input) const;
        
        bool sequence_accepts(const parse_sequence& current, const char c) const;
                
        
        
        std::vector<parse_sequence> sequence_increment(const parse_sequence& current) const;
        
        std::vector<parse_sequence> node_initial_expansion(const node& n) const;
        
        std::vector<parse_sequence> merge_sequences(const std::vector<parse_sequence>& s0, const std::vector<parse_sequence>& s1) const;
        std::vector<parse_sequence> add_sequences(const parse_sequence& current, const std::vector<parse_sequence>& s0) const;
        
        void print(const std::vector<parse_sequence>& seqvec) const;
        
        
        node_generator m_generator;
        std::vector<node> m_nodes;
        std::vector<std::string> m_top_rules;
        std::vector<parse_sequence> m_initial_sequence;
        std::unordered_map<std::string, size_t> m_rule_map;
        std::unordered_map<size_t, std::string> m_name_map;
    };
}
