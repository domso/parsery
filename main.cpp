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
                text += line;
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
        auto tree = p.build_tree(blub.fully_accepted, text);
        tree.print(0);
    }
    
private:
    
};




int main(int argc, char **argv) {
    /*
    grammar_reader gr;
    gr.read("../1076-2019.txt");
    return 0;
    */
    parser::parser p;
    
    p.add_rule("label", "(a-z|A-Z)(a-z|A-Z|0-9)*");
    p.add_rule("number", "(0-9)(0-9)*");
    p.add_rule("operator", "(+|\\-|\\*|/|=)");
    p.add_top_rule("term", "([label]|[number]) *[operator] *([term]|[number]|[label])");
     
    p.init();
                                     
    auto text = "1 + 2 * 3";
    auto blub = p.parse_to_sequence(text);
    std::cout << "fully" << std::endl;
    p.build_tree(blub.fully_accepted, text).print(0);
    std::cout << "partial" << std::endl;
    p.build_tree(blub.partial_accepted, text).print(0);
    std::cout << "rejected" << std::endl;
    p.build_tree(blub.rejected, text).print(0);

    return 0;
}
