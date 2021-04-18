#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <unordered_map>
#include <unordered_set>

enum token_types {
    KEY_WORD_LIBRARY,
    KEY_WORD_ENTITY,
    KEY_WORD_IS,
    KEY_WORD_GENERIC, 
    KEY_WORD_PORT,
    KEY_WORD_IN,
    KEY_WORD_OUT,
    KEY_WORD_END,
    KEY_WORD_ARCHITECTURE,
    KEY_WORD_FUNCTION,
    KEY_WORD_RETURN,
    KEY_WORD_BEGIN,
    KEY_WORD_PROCEDURE,
    KEY_WORD_INOUT,
    KEY_WORD_PROCESS,
    KEY_WORD_ASSERT,
    KEY_WORD_REPORT,
    KEY_WORD_SEVERITY,
    KEY_WORD_NOTE,
    KEY_WORD_WAIT,
    KEY_WORD_ON, 
    KEY_WORD_WHILE,
    KEY_WORD_FOR,
    KEY_WORD_LOOP,
    KEY_WORD_IF,
    KEY_WORD_THEN,
    KEY_WORD_ELSE,
    KEY_WORD_ELSIF,
    KEY_WORD_CASE,
    KEY_WORD_WHEN,
    KEY_WORD_OTHERS,
    KEY_WORD_GENERATE,
    KEY_WORD_MAP,
    KEY_WORD_USE,
    KEY_WORD_TO,
    KEY_WORD_DOWNTO,
    KEY_WORD_OF,
    KEY_WORD_PACKAGE,
    KEY_WORD_BODY,
    KEY_WORD_TYPE,
    KEY_WORD_RECORD,
    KEY_WORD_ALL,
    KEY_WORD_AND,
    KEY_WORD_OR,
    KEY_WORD_XOR,
    KEY_WORD_NOT,
    KEY_WORD_SLL,
    KEY_WORD_SRL,
    KEY_WORD_SLA,
    KEY_WORD_SRA,
    KEY_WORD_SIGNAL,
    KEY_WORD_VARIABLE,
    KEY_WORD_RANGE,
    KEY_WORD_COMPONENT,
    KEY_WORD_CONSTANT,
    KEY_WORD_SUBTYPE,
        
    OPERATOR_DOUBLE_POINT,
    OPERATOR_SEMICOLON,
    OPERATOR_COMMA,
    OPERATOR_BRACKET_OPEN,
    OPERATOR_BRACKET_CLOSE,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVISION,
    OPERATOR_SINGLE_QUOTE,
    OPERATOR_DOUBLE_QUOTE,
    OPERATOR_SMALLER,
    OPERATOR_GREATER,
    OPERATOR_EQUALS,
    OPERATOR_POINT,
    COMMENT,
    EXPRESSION
};                      
                      
class token_classificator {
public:    
    token_classificator() {
        m_token_map["library"] = KEY_WORD_LIBRARY;
        m_token_map["entity"] = KEY_WORD_ENTITY;
        m_token_map["is"] = KEY_WORD_IS;
        m_token_map["generic"] = KEY_WORD_GENERIC;
        m_token_map["port"] = KEY_WORD_PORT;
        m_token_map["in"] = KEY_WORD_IN;
        m_token_map["out"] = KEY_WORD_OUT;
        m_token_map["end"] = KEY_WORD_END;
        m_token_map["architecture"] = KEY_WORD_ARCHITECTURE;
        m_token_map["function"] = KEY_WORD_FUNCTION;
        m_token_map["return"] = KEY_WORD_RETURN;
        m_token_map["begin"] = KEY_WORD_BEGIN;
        m_token_map["procedure"] = KEY_WORD_PROCEDURE;
        m_token_map["inout"] = KEY_WORD_INOUT;
        m_token_map["process"] = KEY_WORD_PROCESS;
        m_token_map["assert"] = KEY_WORD_ASSERT;
        m_token_map["report"] = KEY_WORD_REPORT;
        m_token_map["severity"] = KEY_WORD_SEVERITY;
        m_token_map["note"] = KEY_WORD_NOTE;
        m_token_map["wait"] = KEY_WORD_WAIT;
        m_token_map["on"] = KEY_WORD_ON;
        m_token_map["while"] = KEY_WORD_WHILE;
        m_token_map["for"] = KEY_WORD_FOR;
        m_token_map["loop"] = KEY_WORD_LOOP;
        m_token_map["if"] = KEY_WORD_IF;
        m_token_map["then"] = KEY_WORD_THEN;
        m_token_map["else"] = KEY_WORD_ELSE;
        m_token_map["elsif"] = KEY_WORD_ELSIF;
        m_token_map["case"] = KEY_WORD_CASE;
        m_token_map["when"] = KEY_WORD_WHEN;
        m_token_map["others"] = KEY_WORD_OTHERS;
        m_token_map["generate"] = KEY_WORD_GENERATE;
        m_token_map["map"] = KEY_WORD_MAP;
        m_token_map["use"] = KEY_WORD_USE;
        m_token_map["to"] = KEY_WORD_TO;
        m_token_map["downto"] = KEY_WORD_DOWNTO;
        m_token_map["of"] = KEY_WORD_OF;
        m_token_map["package"] = KEY_WORD_PACKAGE;
        m_token_map["body"] = KEY_WORD_BODY;
        m_token_map["type"] = KEY_WORD_TYPE;
        m_token_map["record"] = KEY_WORD_RECORD;
        m_token_map["all"] = KEY_WORD_ALL;
        m_token_map["and"] = KEY_WORD_AND,
        m_token_map["or"] = KEY_WORD_OR,
        m_token_map["xor"] = KEY_WORD_XOR,
        m_token_map["not"] = KEY_WORD_NOT,
        m_token_map["sll"] = KEY_WORD_SLL,
        m_token_map["srl"] = KEY_WORD_SRL,
        m_token_map["sla"] = KEY_WORD_SLA,
        m_token_map["sra"] = KEY_WORD_SRA,
        m_token_map["signal"] = KEY_WORD_SIGNAL,
        m_token_map["variable"] = KEY_WORD_VARIABLE,
        m_token_map["range"] = KEY_WORD_RANGE,
        m_token_map["component"] = KEY_WORD_COMPONENT,
        m_token_map["constant"] = KEY_WORD_CONSTANT,
        m_token_map["subtype"] = KEY_WORD_SUBTYPE,
        
        
        m_token_map[":"] = OPERATOR_DOUBLE_POINT;
        m_token_map[";"] = OPERATOR_SEMICOLON;
        m_token_map[","] = OPERATOR_COMMA;
        m_token_map["("] = OPERATOR_BRACKET_OPEN;
        m_token_map[")"] = OPERATOR_BRACKET_CLOSE;
        m_token_map["+"] = OPERATOR_PLUS;
        m_token_map["-"] = OPERATOR_MINUS;
        m_token_map["*"] = OPERATOR_MULTIPLY;
        m_token_map["/"] = OPERATOR_DIVISION;
        m_token_map["'"] = OPERATOR_SINGLE_QUOTE;
        m_token_map["\""] = OPERATOR_DOUBLE_QUOTE;
        m_token_map["<"] = OPERATOR_SMALLER;
        m_token_map[">"] = OPERATOR_GREATER;
        m_token_map["="] = OPERATOR_EQUALS;
        m_token_map["."] = OPERATOR_POINT; 
        
        m_reverse_token_map[COMMENT] = "COMMENT";
        
        for (auto& e : m_token_map) {
            m_reverse_token_map[e.second] = e.first;
        }        
    }    
    
    token_types classify(const std::string& text) const {
        
        if (text.find("--") != std::string::npos) {
            return COMMENT;
        }
        
        if (m_token_map.count(text) > 0) {
            return m_token_map.at(text);
        }
        
        return EXPRESSION;
    }
    
    std::string name(const token_types type) const {           
        if (m_reverse_token_map.count(type) > 0) {
            return m_reverse_token_map.at(type);
        }
        
        return "INVALID";
    }
private:
    std::unordered_map<std::string, token_types> m_token_map;
    std::unordered_map<token_types, std::string> m_reverse_token_map;
};

class token {
public:    
    token(const std::string& text, const size_t line) : m_text(text), m_line(line) {}

    const std::string& text() const {
        return m_text;
    }

    size_t line() const {
        return m_line;
    }
    
    void set_type(const token_types type) {
        m_type = type;
    }
    
    token_types type() const {
        return m_type;
    }

    void set_hierarchy(const int hierarchy) {
        m_hierarchy = hierarchy;
    }
    
    int hierarchy() const {
        return m_hierarchy;
    }    
private:
    token_types m_type;
    std::string m_text;
    size_t m_line;
    int m_hierarchy = 0;
};

std::string remove_whitespace(const std::string& text) {
    std::string result;
    bool last = true;

    for (size_t i = 0; i < text.length(); i++) {
        if (text.at(i) == ' '  ||
                text.at(i) == '\n' ||
                text.at(i) == '\r' ||
                text.at(i) == '\t'
           ) {
            if (!last) {
                result = result + text.at(i);
                last = true;
            }
        } else {
            result = result + text.at(i);
            last = false;
        }
    }


    return result;
}

std::vector<token> split_line(const std::string& line, const size_t line_number) {
    std::vector<token> result;
    
    size_t current = 0;
    size_t pos;

    if (line != "") {
        do {
            std::string token_text;
            pos = line.find_first_of(" :;,()+-*/'\"<>=.", current);
            if (current == pos) {
                token_text = line.substr(current, 1);
                current = pos + 1;
            } else {
                if (pos != std::string::npos) {
                    token_text = line.substr(current, pos - current);
                } else {
                    token_text = line.substr(current);
                }
                current = pos;
            }    
                        
            if (token_text != " " && token_text != "") {
                token t(token_text, line_number);
                result.push_back(t);
            }                
        } while (current < line.length() && pos != std::string::npos);
    }

    return result;
}

std::vector<token> split_file(const std::string& filename) {
    std::vector<token> result;

    std::ifstream infile(filename);
    std::string line;
    size_t line_number = 1;

    if (infile.is_open()) {
        while (std::getline(infile, line)) {
            std::string clean_line = remove_whitespace(line);
            std::vector<token> line_tokens;
            auto pos = clean_line.find("--");

            if (pos == std::string::npos) {
                line_tokens = split_line(clean_line, line_number);
            } else {
                line_tokens = split_line(clean_line.substr(0, pos), line_number);
                token t(clean_line.substr(pos), line_number);
                line_tokens.push_back(t);
            }
            line_number++;

            for (auto& v : line_tokens) {
                result.push_back(v);
            }
        }
    }

    return result;
}



bool set_hierarchy_for_tokens(std::vector<token>& tokens) {
    
    
  
    std::vector<std::vector<token_types>> sequences = {
        {KEY_WORD_LIBRARY, OPERATOR_SEMICOLON},
        {KEY_WORD_USE, OPERATOR_SEMICOLON},
        {OPERATOR_BRACKET_OPEN, OPERATOR_BRACKET_CLOSE},
        {KEY_WORD_GENERIC, OPERATOR_BRACKET_OPEN, OPERATOR_BRACKET_CLOSE},
        {KEY_WORD_PORT, OPERATOR_BRACKET_OPEN, OPERATOR_BRACKET_CLOSE},
        {OPERATOR_DOUBLE_QUOTE, OPERATOR_DOUBLE_QUOTE},
        {KEY_WORD_ENTITY, KEY_WORD_IS, KEY_WORD_END, KEY_WORD_ENTITY, OPERATOR_SEMICOLON},
        {KEY_WORD_ENTITY, KEY_WORD_IS, KEY_WORD_END, EXPRESSION, OPERATOR_SEMICOLON},
        {KEY_WORD_ENTITY, OPERATOR_SEMICOLON},
        {KEY_WORD_PACKAGE, KEY_WORD_IS, KEY_WORD_END, KEY_WORD_PACKAGE, OPERATOR_SEMICOLON},
        {KEY_WORD_PACKAGE, KEY_WORD_IS, KEY_WORD_END, EXPRESSION, OPERATOR_SEMICOLON},
        {KEY_WORD_PACKAGE, KEY_WORD_BODY, KEY_WORD_IS, KEY_WORD_END, KEY_WORD_PACKAGE, OPERATOR_SEMICOLON},
        {KEY_WORD_PACKAGE, KEY_WORD_BODY, KEY_WORD_IS, KEY_WORD_END, EXPRESSION, OPERATOR_SEMICOLON},
        {KEY_WORD_ARCHITECTURE, KEY_WORD_IS, KEY_WORD_BEGIN, KEY_WORD_END, KEY_WORD_ARCHITECTURE, OPERATOR_SEMICOLON},
        {KEY_WORD_ARCHITECTURE, KEY_WORD_IS, KEY_WORD_BEGIN, KEY_WORD_END, EXPRESSION, OPERATOR_SEMICOLON},
        {KEY_WORD_PROCESS, KEY_WORD_BEGIN, KEY_WORD_END, KEY_WORD_PROCESS, OPERATOR_SEMICOLON},
        {KEY_WORD_IF, KEY_WORD_THEN, KEY_WORD_END, KEY_WORD_IF, OPERATOR_SEMICOLON},
        {KEY_WORD_IF, KEY_WORD_THEN, KEY_WORD_ELSE, KEY_WORD_END, KEY_WORD_IF, OPERATOR_SEMICOLON},
        {KEY_WORD_ELSIF, KEY_WORD_THEN},
        {KEY_WORD_WHILE, KEY_WORD_LOOP, KEY_WORD_END, KEY_WORD_LOOP, OPERATOR_SEMICOLON},
        {KEY_WORD_FOR, KEY_WORD_LOOP, KEY_WORD_END, KEY_WORD_LOOP, OPERATOR_SEMICOLON},
        {KEY_WORD_CASE, KEY_WORD_IS, KEY_WORD_END, KEY_WORD_CASE, OPERATOR_SEMICOLON},
        {KEY_WORD_WHEN, OPERATOR_EQUALS, OPERATOR_GREATER, OPERATOR_SEMICOLON},
        {KEY_WORD_WHEN, KEY_WORD_ELSE, OPERATOR_SEMICOLON},
        {KEY_WORD_COMPONENT, KEY_WORD_IS, KEY_WORD_END, KEY_WORD_COMPONENT, OPERATOR_SEMICOLON},
        {KEY_WORD_TYPE, KEY_WORD_IS, KEY_WORD_RECORD, KEY_WORD_END, KEY_WORD_RECORD, OPERATOR_SEMICOLON},
        {KEY_WORD_TYPE, KEY_WORD_IS, OPERATOR_SEMICOLON},
        {KEY_WORD_SUBTYPE, KEY_WORD_IS, OPERATOR_SEMICOLON},
        {KEY_WORD_FUNCTION, KEY_WORD_RETURN, OPERATOR_SEMICOLON},
        {KEY_WORD_FUNCTION, OPERATOR_BRACKET_OPEN, OPERATOR_BRACKET_CLOSE, KEY_WORD_RETURN, OPERATOR_SEMICOLON},
        {KEY_WORD_FUNCTION, KEY_WORD_RETURN, KEY_WORD_IS, KEY_WORD_BEGIN, KEY_WORD_END, KEY_WORD_FUNCTION, OPERATOR_SEMICOLON},
        {KEY_WORD_FUNCTION, OPERATOR_BRACKET_OPEN, OPERATOR_BRACKET_CLOSE, KEY_WORD_RETURN, KEY_WORD_IS, KEY_WORD_BEGIN, KEY_WORD_END, KEY_WORD_FUNCTION, OPERATOR_SEMICOLON},
        {KEY_WORD_PROCEDURE, OPERATOR_SEMICOLON},
        {KEY_WORD_PROCEDURE, OPERATOR_BRACKET_OPEN, OPERATOR_BRACKET_CLOSE,OPERATOR_SEMICOLON},
        {KEY_WORD_PROCEDURE, KEY_WORD_IS, KEY_WORD_BEGIN, KEY_WORD_END, KEY_WORD_PROCEDURE, OPERATOR_SEMICOLON},
        {KEY_WORD_PROCEDURE, OPERATOR_BRACKET_OPEN, OPERATOR_BRACKET_CLOSE, KEY_WORD_IS, KEY_WORD_BEGIN, KEY_WORD_END, KEY_WORD_PROCEDURE, OPERATOR_SEMICOLON},
        {KEY_WORD_SIGNAL, OPERATOR_DOUBLE_POINT, OPERATOR_SEMICOLON},
        {KEY_WORD_SIGNAL, OPERATOR_DOUBLE_POINT, OPERATOR_DOUBLE_POINT, OPERATOR_EQUALS, OPERATOR_SEMICOLON},
        {KEY_WORD_VARIABLE, OPERATOR_DOUBLE_POINT, OPERATOR_SEMICOLON},
        {KEY_WORD_VARIABLE, OPERATOR_DOUBLE_POINT, OPERATOR_DOUBLE_POINT, OPERATOR_EQUALS, OPERATOR_SEMICOLON},
        {KEY_WORD_CONSTANT, OPERATOR_DOUBLE_POINT, OPERATOR_DOUBLE_POINT, OPERATOR_EQUALS, OPERATOR_SEMICOLON},
    };
    
    std::vector<std::pair<std::vector<size_t>, size_t>> stack;
    int current = 0;
    
    for (auto& t : tokens) {                
        if (stack.empty()) {
            t.set_hierarchy(current);
            std::pair<std::vector<size_t>, size_t> entry;
            entry.second = 1;
            
            for (size_t i = 0; i < sequences.size(); i++) {
                if (sequences[i][0] == t.type()) {
                    entry.first.push_back(i);
                }
            }
            
            if (!entry.first.empty()) {
                current++;
                stack.push_back(entry);
            }
        } else {
            auto& stack_top = stack.at(stack.size() - 1);
            bool found = false;
            std::pair<std::vector<size_t>, size_t> entry;
            
            for (auto& e : stack_top.first) {
                if (sequences[e][stack_top.second] == t.type()) {
                    if (sequences[e].size() == stack_top.second + 1) {
                        found = true;
                        break;
                    } else {
                        entry.first.push_back(e);
                        entry.second = stack_top.second + 1;
                    }
                }
            }
            
            if (found) {
                current--;
                t.set_hierarchy(current);
                stack.pop_back();
            } else {
                if (!entry.first.empty()) {
                    t.set_hierarchy(current - 1);
                    stack_top = entry;
                } else {
                    t.set_hierarchy(current);
                    entry.second = 1;
                    
                    for (size_t i = 0; i < sequences.size(); i++) {
                        if (sequences[i][0] == t.type()) {
                            entry.first.push_back(i);
                        }
                    }
                    
                    if (!entry.first.empty()) {
                        current++;
                        stack.push_back(entry);
                    }                    
                }
            }            
        }        
    }
    
    
    
    return current == 0;
//     std::unordered_set<token_types> start_tokens = {
//         KEY_WORD_IS, 
//         KEY_WORD_RECORD,
//         KEY_WORD_IF,
//         KEY_WORD_FOR,
//         KEY_WORD_WHILE,
//         KEY_WORD_PROCESS,
//         KEY_WORD_CASE,
//         OPERATOR_BRACKET_OPEN
//     };    
//     
//     std::unordered_set<token_types> end_tokens = {KEY_WORD_END, OPERATOR_BRACKET_CLOSE, KEY_WORD_ELSIF};
//     std::unordered_set<token_types> sep_tokens = {KEY_WORD_BEGIN, KEY_WORD_ELSE, KEY_WORD_LOOP, KEY_WORD_THEN, KEY_WORD_GENERATE};
//     std::unordered_map<token_types, bool> switch_tokens = {{OPERATOR_SINGLE_QUOTE, false}, {OPERATOR_DOUBLE_QUOTE, false}};  
//     
//     int current = 0;    
//     token_types last = KEY_WORD_ALL;
//     
//     for (auto& t : tokens) {        
//         if (start_tokens.count(t.type())) {            
//             if ((t.type() == KEY_WORD_IS && last == KEY_WORD_TYPE) || last == KEY_WORD_END) {
//                 t.set_hierarchy(current);
//             } else {
//                 t.set_hierarchy(current);
//                 current++;
//             }
//         } else if (end_tokens.count(t.type())) {
//             current--;
//             t.set_hierarchy(current);
//         } else if (sep_tokens.count(t.type())) {
//             t.set_hierarchy(current - 1);
//         } else if (switch_tokens.count(t.type())) {
//             if (!switch_tokens.at(t.type())) {
//                 t.set_hierarchy(current);
//                 current++;                
//                 switch_tokens[t.type()] = true;       
//             } else {         
//                 current--;                
//                 t.set_hierarchy(current);
//                 switch_tokens[t.type()] = false;
//             }
//         } else {
//             t.set_hierarchy(current);
//         }     
//         
//         
//         if (t.type() != EXPRESSION) {
//             last = t.type();
//         }        
//     }    
}


void entity_parser(const std::vector<token>& tokens, const std::pair<size_t, size_t> range, const size_t depth) {
//     assert(tokens[range.first].type() == KEY_WORD_ENTITY);
    
    
    
    
    
    
}


void print_tokens(const std::vector<token>& tokens, const token_classificator& classifier) {
    size_t current_line = 1;
    
    for (auto& t : tokens) {
        if (t.line() != current_line) {
            std::cout << std::endl;
            current_line = t.line();
        }
        
        if (t.type() == EXPRESSION) {
            std::cout << t.text();
        } else {
            std::cout << "[" << classifier.name(t.type()) << "]";
        }
        
    }    
    
    std::cout << std::endl;
}

void print_hierarchy(const std::vector<token>& tokens, const token_classificator& classifier) {
    for (auto& t : tokens) {
        
        for (int i = 0; i < t.hierarchy(); i++) {
            std::cout << "   ";
        }
        
        if (t.type() == EXPRESSION) {
            std::cout << t.text() << std::endl;
        } else {
            std::cout << "[" << classifier.name(t.type()) << "]" << std::endl;
        }
    }
}

#include "src/parser/node_generator.h"
#include "src/parser/parser.h"

int main(int argc, char **argv) {
//     if (argc == 2) {

    parser::parser p;
        
    
    p.add_rule("label", "(a-z|A-Z)(a-z|A-Z|0-9)*");
    p.add_rule("number", "(0-9)(0-9)*");
    p.add_rule("operator", "(+|+)");
    p.add_top_rule("term", "([number]|[label])[operator]([number]|[label])([operator]([number]|[label]))*;");
    
    p.init();
    
    auto blub = p.parse_to_sequence("123+321;");
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Solution" << std::endl;
    p.print(blub);
    
//     }

    return 0;
}
