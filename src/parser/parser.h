#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include "node_generator.h"

namespace parser {
    class parser {
    public:
        void add_rule(const std::string& name, const std::string& rule);
        void add_top_rule(const std::string& name, const std::string& rule);
        
        void init();
        
        struct history_element {
            size_t rule;
            bool open;
            size_t section;
            bool operator==(const history_element& other) const;
        };
        
        struct parse_sequence {
            std::vector<size_t> path;
            std::vector<history_element> history;
            bool operator==(const parse_sequence& other) const;
        };
        
        struct solution {
            std::vector<parse_sequence> fully_accepted;
            std::vector<parse_sequence> partial_accepted;
            std::vector<parse_sequence> rejected;
            size_t rejected_index;
        };
        
        solution parse_to_sequence(const std::string& input);
        
        void print(const std::vector<parse_sequence>& seqvec, const std::string input) const;
    private:
        struct parse_context {
            parser::parser::solution result;
            std::vector<parser::parser::parse_sequence> sequence;
            std::vector<history_element> partial_history;
        };
        bool check_character(const size_t pos, const size_t n, const char c, parse_context& context) const;


        bool sequence_accepts(const parse_sequence& current, const char c) const;
        
        std::vector<parse_sequence> sequence_increment(const parse_sequence& current) const;        
        std::vector<parse_sequence> node_initial_expansion(const node& n) const;        
        std::vector<parse_sequence> merge_sequences(const std::vector<parse_sequence>& s0, const std::vector<parse_sequence>& s1, const size_t n0, const size_t n1) const;
        std::vector<parse_sequence> add_sequences(parse_sequence& current, const std::vector<parse_sequence>& s0) const;
        
        template<typename T>
        bool contains(const std::vector<T>& vec, const T& o) const {
            for (const auto& v : vec) {
                if (v == o) {
                    return true;
                }
            }
            
            return false;
        }

        template<typename T>
        void append_vector(std::vector<T>& first, const std::vector<T>& second) const {
            first.reserve(first.size() + second.size());
            for (auto& s : second) {
                first.push_back(std::move(s));
            }
        }
        
        node_generator m_generator;
        std::vector<node> m_nodes;
        std::vector<std::string> m_top_rules;
        std::vector<parse_sequence> m_initial_sequence;
        std::unordered_map<std::string, size_t> m_rule_map;
        std::unordered_map<size_t, std::string> m_name_map;
    };
}
