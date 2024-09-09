# Parsery

Parsery is a minimalistic, powerful, and easy-to-use parser library written in C++.
It allows you to define grammar rules and parse strings.

## Rules
 - A rule consists of a non-terminal symbol (left-hand side) and a sequence of terminal and/or non-terminal symbols (right-hand side).
 - Rules define the grammar of the language you want to parse.

## Top Rule
 - The top rule specifies the root of the parse tree.

## Grammar Syntax

The right-hand side of a rule supports various constructs common in formal grammars:

1. **Character Classes**: (a-z) - Matches any single character in the specified range.

2. **Kleene Star**: E* - Matches zero or more occurrences of the expression E.

3. **Alternation**: E1 | E2 - Matches either E1 or E2.

4. **Grouping**: (E1 E2) | E3 - Groups elements together. In this example, it matches either the sequence E1 E2 or E3.

5. **Non-terminal References**: [A] [B] - Matches the sequence of non-terminals [A] and [B].

6. **Escaping**: \s - Matches the character s, required for '*', '-', '(', ')', '[', ']', '|'.

Recursive references are allowed.
