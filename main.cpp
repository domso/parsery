#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <unordered_map>
#include <unordered_set>

#include "src/parser/node_generator.h"
#include "src/parser/parser.h"

class grammar_reader {
public:
    void read(const std::string& filename) {
        std::ifstream file(filename);
        std::string text;
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                for (const auto c : line) {
                    if (c != '\n') {
                        text += c;
                    }
                }
            }
//             for (int i = 0; i < 1500; i++) {
//                 std::getline(file, line);
//                 for (const auto c : line) {
//                     if (c != '\n') {
//                         text += c;
//                     }
//                 }
//             }
        }    
        
        
        
        
        std::cout << text<< std::endl;
        parser::parser p;
        
//         p.add_top_rule("rule", "(a-z|A-Z|0-9|{|}|_|.|,|;|:|#| |=|!|+|/|<|>|&|^|'|\\||\\*|\\-|\"|\\]|\\[|\\))*");
//         
//         
//         p.init();
//         
//         auto k = p.parse_to_sequence("y)");
//         std::cout << "Part" << std::endl;
//         p.print(k.partial_accepted);
//         std::cout << "Solution" << std::endl;
//         p.print(k.fully_accepted);
//         
//         return;
                        
        p.add_rule("number", "(0-9)(0-9)*");
        p.add_rule("paragraph", "[number](.[number])*");
        p.add_rule("reference", "\\[§ *[paragraph]\\]");
        p.add_rule("subname", "(a-z|A-Z)(a-z|A-Z)*");
        p.add_rule("name", "[subname](_[subname])*");
        p.add_rule("rule", "(( -#)|(%-~))*");
        
        
        
        
        p.add_top_rule("rule_def", "[name] *::=[rule][reference]");
        
        p.init();
                                        
        
        auto blub = p.parse_to_sequence(text);
        std::cout << "Part" << std::endl;
        p.print(blub.partial_accepted);
        std::cout << "Solution" << std::endl;
        p.print(blub.fully_accepted);
    }
    
private:
    
};




int main(int argc, char **argv) {
//     if (argc == 2) {
    
    grammar_reader gr;
    gr.read("../1076-2019.txt");
    
/*
    parser::parser p;
    
    p.add_rule("label", "(a-z|A-Z)(a-z|A-Z|0-9)*");
    p.add_rule("number", "(0-9)(0-9)*");
    p.add_rule("operator", "(+|\\-|\\*|/|=)");
    p.add_top_rule("term", "([number]|[label]) *[operator] *([number]|[label])( *[operator] *([number]|[label]))*");
    p.add_top_rule("assignment", "[label] *= *[term]");
    
    p.add_top_rule("bracket_term", "(\\([term]\\))|[term]|[number]|[label]");
    p.add_top_rule("bracket_add_term", "[bracket_term] *[operator] * [bracket_term] *( *[operator] *[bracket_term])*;");
     
    p.init();
                                     

    auto blub = p.parse_to_sequence("a5");
    std::cout << "Solution" << std::endl;
    p.print(blub);*/

    return 0;
}
