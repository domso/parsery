#include <iostream>

#include "parser.h"

int main(int argc, char **argv) {
    parsery::parser p;

    p.add_rule("ws", " *");
    p.add_rule("number", "(0-9)*((.(0-9)*)|)((e(0-9)(0-9)*)|)");
    p.add_rule("operator", "+|(\\-)|(\\*)|(/)");
    p.add_rule("enclosed_term", "[ws]\\([ws][term][ws]\\)[ws]");
    p.add_rule("term", "[ws]([enclosed_term]|[number]|([term][ws][operator][ws][term]))[ws]");
    p.add_top_rule("top", "[term]");

    int n = 0;
    p.parse("654.4e5 + (5 + (12.5 * 4))", [&](const auto& scope) {
        std::cout << std::string(4 * n, ' ') << "<" << scope << ">" << std::endl;
        n++;
    }, [&](const auto& scope) {
        n--;
        std::cout << std::string(4* n, ' ') << "</" << scope << ">" << std::endl;
    }, [&](const auto& scope) {
        std::cout << std::string(4 * n, ' ') << scope << std::endl;
    });


    return 0;
}
