#include <iostream>
#include <vector>
#include <string>
#include <fstream>


/*

absolute_pathname .[partial_pathname]
abtract_literal [decimal_literal]|[based_literal]
access_type_definition access [subtype_indication]

actual_designator ((inertial)*[expression])|[name]|[subtype_indication]|[open]

actual_parameter_part [association_list]

actual_part [actual_designator]|([name]\([actual_designator]\))|([type_mark]\([actual_designator]\))

adding_operator +|\-|&

aggregate \([element_association(, [element_association])*]\)

alias_designator [identifier]|[character_literal]|[operator_symbol]

allocator (new [subtype_indication])|(new [qualified_expression])

architecture_body architecture [identifier] of [name] is [architecture_declarative_part] begin [architecture_statement_part] [end](architecture)*[name]*;

architecture_declarative_part [block_declarative_item]*

architecture_statement_part [concurrent_statement]*

array_constraint ([index_constraint]([array_element_constraint])*)|(\(open\)[array_element_constraint]*)

array_element_constraint [element_constraint]

array_element_resolution [resolution_indication]

array_type_definition [unbounded_array_definition]|[constrained_array_definition]

assertion assert [condition] (report [expression])*(severity [expression])*

assertion_statement ([label]:)[assertion];

association_element ([formal_part] =>)* [actual_part]

association_list [association_element](, [association_element])*


attribute_declaration attribute [identifier] : [type_mark];

attribute_designator [simple_name]

attribute_name [prefix]([signature]*)'[attribute_designator](\([expression]\))*

attribute_specification attribute [attribute_designator] of [entity_specification] is [expression];

base [integer]

base_specifier B|O|X|(UB)|(UO)|(UX)|(SB)|(SO)|(SX)|D

based_integer [extended_digit]([underline]*[extended_digit])*

based_literal [base]#[based_integer](.[based_integer])*#[exponent]*

basic_character [basic_graphic_character]|[format_effector]


*/

#include "src/parser/node_generator.h"
#include "src/parser/parser.h"

int main(int argc, char **argv) {
//     if (argc == 2) {

    parser::parser p;
    
    p.add_rule("label", "(a-z|A-Z)(a-z|A-Z|0-9)*");
    p.add_rule("number", "(0-9)(0-9)*");
    p.add_rule("operator", "(+|\\-|\\*|/|=)");
    p.add_top_rule("term", "([number]|[label]) *[operator] *([number]|[label])( *[operator] *([number]|[label]))*");
    p.add_top_rule("assignment", "[label] *= *[term]");
    
    p.add_top_rule("bracket_term", "(\\([term]\\))|[term]|[number]|[label]");
    p.add_top_rule("bracket_add_term", "[bracket_term] *[operator] * [bracket_term] *( *[operator] *[bracket_term])*;");
     
    p.init();
                                     
    auto blub = p.parse_to_sequence("(a*0)");
    
    if (!blub.fully_accepted.empty()) {        
        std::cout << "Found Solution" << std::endl;
        p.print(blub.fully_accepted);
    } else if (!blub.partial_accepted.empty()) {        
        std::cout << "Unexspected End of File" << std::endl;
        p.print(blub.partial_accepted);
    } else {
        std::cout << "Unexspected character" << std::endl;
        p.print(blub.rejected);        
    }
    
    

    return 0;
}
