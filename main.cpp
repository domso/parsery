#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>

#include <unordered_map>
#include <unordered_set>

#include "src/parser.h"

#include <optional>
#include <memory>
#include <algorithm>







class white_space_trimmer {
public:
    static std::string trim(const std::string& text) {
        bool maybe_spaced = false;
        bool requires_space = false;
        std::string result;
     
        for (const auto c : text) {
            if (is_to_be_separated(c)) {
                if (requires_space) {
                    result += ' ';
                    requires_space = false;
                }
                result += c;
                maybe_spaced = true;                
            } else if (is_whitespace(c)) {
                if (maybe_spaced) {
                    requires_space = true;
                }                
            } else {
                result += c;
                maybe_spaced = false;
                requires_space = false;
            }
        }

        return result;
    }
private:
    static bool is_to_be_separated(const char c) {
        return ('a' <= c && c <= 'z') || ('A' <= c && c < 'Z') || ('0' <= c && c <= '9');
    }
    static bool is_whitespace(const char c) {
        return (c == ' ') || (c == '\n') || (c == '\r') || (c == '\t');
    }
};


std::string readin_file(const std::string filename) {
    auto file = std::ifstream(filename);
    std::string result;
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            result += line;
        }
    }
    return result;
}

int main2(int argc, char **argv) {
    parser::parser p;


    //p.add_rule("number", "(0-9)(0-9)*");
    //p.add_rule("name", "(a-z|A-Z)(a-z|A-Z)*");

    //p.add_rule("attribute", "[number]|[name]|[object]|[array]");
    //p.add_rule("array", "\\[[attribute](,[attribute])*\\]");

    //p.add_rule("named_attribute", "\"[name]\":[attribute]");
    //p.add_top_rule("object", "({})|({[named_attribute](,[named_attribute])*})");

    //p.add_rule("text.number.1", "(0-9)");
    //p.add_rule("data.number.1", "(0-9)");

    //p.add_rule("ascii", "(\x0-\x7F)");
    //p.add_rule("utf8_2", "(\xC2-\xDF)(\x80-\xBF)(\x80-\xBF)");
    //p.add_rule("utf8_3", "(\xE0-\xEF)(\x80-\xBF)(\x80-\xBF)(\x80-\xBF)");
    //p.add_rule("utf8_4", "(\xF0-\xF4)(\x80-\xBF)(\x80-\xBF)(\x80-\xBF)(\x80-\xBF)");

    //p.add_rule("utf8", "((\x0-\x21)|(\x23-\x5B)|(\x5D-\xFF)|((\x0-\xFF)))*");
    p.add_rule("utf8", "((\x1-\x21)|(\x23-\\\x5B)|(\\\x5D-\xFF)|(\x5C\x5C(\x1-\xFF)))*");

    //p.add_rule("number", "(((1-9)(0-9)*)|0)(|(.(0-9)(0-9)*(|((e|E)(+|\\-)(0-9)(0-9)*))))");
    p.add_rule("number", "(((1-9)(0-9)*)|0)((.(0-9)(0-9)*(((e|E)(+|\\-|)(0-9)(0-9)*)|))|)");

    //p.add_rule("asd", "\"asd\":[text.number.1]");
    //p.add_rule("data", "\"data\":\\[[data.number.1]\\]");

    //p.add_rule("top_attr", "[asd]|[data]");
    
    //p.add_top_rule("top", "{\"asd\":[text.number.1],\"data\":\\[[text.number.1]\\]}");
    p.add_top_rule("top", "\"[number]\"");

    p.parse("\"654.4e5\"", [](const auto& scope) {
        std::cout << scope << std::endl;
    }, [](const auto& scope) {
        std::cout << scope << std::endl;
    }, [](const auto& scope) {
        std::cout << scope << std::endl;
    });


    return 0;
}
int main(int argc, char **argv) {
    parser::parser p;



        p.add_rule("number", "(0-9)(0-9)*");
        p.add_rule("paragraph", "[number](.[number])*");
        p.add_rule("reference", "\\[§ *[paragraph]\\]");
        p.add_rule("subname", "(a-z|A-Z)(a-z|A-Z)*");
        p.add_rule("name", "[subname](_[subname])*");

        p.add_rule("text", "((!-#)|(%-Z)|(\\\\)|(^-z))((!-#)|(%-Z)|(\\\\)|(^-z))*");
        p.add_rule("text_sequence", "[text]( [text])*");
        
        p.add_rule("optional", "\\[[or_tree]\\]");
        p.add_rule("list", "{((,|\\|)|)[or_tree]}");

        p.add_rule("or_tree", "[node][node]*(\\|[node][node]*)*");

        p.add_rule("node", "[text_sequence]|[optional]|[list]");


        p.add_rule("rule_def", "[name]::=[or_tree][reference]");
        p.add_top_rule("top", "[rule_def][rule_def]*");


        std::ifstream file("../1076-2019.txt");
        std::string text;

        int n = 0;
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                n++;

                //if (n > 865) {
                //    break;
                //}

                for (const auto c : line) {
                    if (c != '\n') {
                        text += c;
                    }
                }
                text += " ";
            }
        }    

    auto formated_text = white_space_trimmer::trim(text);
    //p.parse(formated_text);
    n = 0;
    p.parse(formated_text, [&](const auto& scope) {
        std::cout << std::string(n, ' ') << "<" << scope << ">" << std::endl;
        n += 1;
    }, [&](const auto& scope) {
        n -= 1;
        std::cout << std::string(n, ' ') << "</" << scope << ">" << std::endl;
    }, [&](const auto& scope) {
        std::cout << std::string(n, ' ') << scope << std::endl;
    });

    return 0;
}
