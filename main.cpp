#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

#include <unordered_map>
#include <unordered_set>

#include "src/parser/node_generator.h"
#include "src/parser/parser.h"

class grammar_reader {
public:
    void read(const std::string& filename) {
        std::ifstream file(filename);
        std::string text;
        int n = 0;
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                n++;
                    if (n == 3) {
                        break;
                    }
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
        p.add_rule("rule_text", "(( -#)|(%-Z)|(\\\\)|(^-z))(( -#)|(%-Z)|(\\\\)|(^-z))*");
        
        p.add_rule("optional", "\\[[rule]\\]");
        p.add_rule("list", "{ ,[rule]}");
        p.add_rule("or", "-[rule]");
        p.add_rule("rule", "([rule_text])|([optional])|([list])|([or])");
        
        
        
        
        p.add_top_rule("rule_def", "[name] *::=[rule][reference]");
        
        p.init();
                                        
        
        auto blub = p.parse_to_sequence(text);
        std::cout << "Part" << std::endl;
        p.print(blub.partial_accepted, text);
        std::cout << "Solution" << std::endl;
        p.print(blub.fully_accepted, text);
    }
    
private:
    
};


#include <fstream>

std::string readin_file(const std::string filename) {
    auto file = std::ifstream(filename);
    std::string result;
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            result += line + "\n";
        }
    }
    
    return result;
}




int main(int argc, char **argv) {    
    
    grammar_reader gr;
    gr.read("../1076-2019.txt");
    
    
    return 0;
    parser::parser p;
    
//     p.add_rule("label", "\"((#-~)|!)*\"");   
//     p.add_rule("dez_number", "(0-9)(0-9)*");
//     p.add_rule("hex_number", "0x(0-9)(0-9)*");
//     p.add_rule("exp_number", "(0-9)e(+|\\-)(0-9)*");
//     p.add_rule("number", "[dez_number]|[hex_number]|[exp_number]");
//     p.add_rule("boolean", "(true)|(false)");
//     p.add_rule("ws", "( |\n)( |\n)*");
//     
//     p.add_rule("array", "(\\[[ws]*\\])|(\\[[ws]*[object]([ws]*,[ws]*[object])*[ws]*\\])|(\\[[ws]*[label]([ws]*,[ws]*[label])*[ws]*\\])|(\\[[ws]*[number]([ws]*,[ws]*[number])*[ws]*\\])|(\\[[ws]*[boolean]([ws]*,[ws]*[boolean])*[ws]*\\])");
//     p.add_rule("attribute", "[label][ws]*:[ws]*([label]|[number]|[boolean]|[object]|[array])");
//     p.add_top_rule("object", "({[ws]*})|({[ws]*[attribute]([ws]*,[ws]*[attribute])*[ws]*})");
//     p.add_top_rule("object", "([label2][ws])");
    
    p.add_rule("label", "\"((#-~)|!)*\"");   
    p.add_rule("ws", "( |\n)( |\n)*");
    p.add_top_rule("object", "");
    
    
    p.init();                                     
   std::string t = readin_file("../test.vhd");
//     t  = "#include ";
    
//     std::string t = "{\"data\":[5";
//     
//     for (int i = 0; i < 1000; i++) {
//         t += "," + std::to_string(i);
//     }
//     
//     t += "]}";
//     std::cout << t << std::endl;
    
//     std::string t = "{\"Herausgeber\":\"Xena\",\"Nummer\":\"1234-5678-9012-3456\",\"Deckung\":2e+6,\"Waehrung\":\"EURO\",\"Inhaber\":{\"Name\":\"Mustermann\",\"Vorname\":\"Max\",\"maennlich\":true,\"Hobbys\":[1,2,3],\"Kinder\":[]}}";

//     t = "{}";
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto blub = p.parse_to_sequence(t);
                                    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    
    std::cout << "Solution found in " << duration << " ns" << std::endl;
//     p.print(blub.fully_accepted, t);   
    
    return 0;    
}
    
    
    
    
    
    
    
    
    
//     if (argc == 2) {
    
//     grammar_reader gr;
//     gr.read("../1076-2019.txt");
    
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


