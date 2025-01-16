# Compiler4031
K.N Toosi University of Technology Compiler 4031 Fall Project

# Lexer
The lexical analyzer generator `flex` is used to tokenize the input streams into tokens in the project specification format.
A symbol table is contained within the implementation of this lexer, to help with error handling in the future.

# Parser
The Parser is made using `bison` and has the symtab.c integrated in it to manage identifiers upon declarations.

## Test Cases
Some Programs written using the program specification, to use for Lexer.
they have been written to test the lexer against and can be found in the `lexer/TestCases` directory.

# TODO


- [] Implement Intermediate Code Generation into parser, translate.y
    - [] Create a Struct to represent quadruples in symtab.h
    - [] Create a way to connect quadruples together -> Linked Lists.
    - [] Everytime a reduction is performed, create and add the corresponding quadruple into Symbol Table.
    - [] In the end, create a way to move through all the quadruples in order and print them into an output.txt file
- [x] Write Syntax Analyzer Generator in yacc/bison.
- [x] Change and Integrate Symbol Table in parser.
- [x] Write Lexical Analyzer Generator in lex/flex.
- [x] Add Manual Lexer Output for the Lexer to be checked against.
- [x] Add Symbol Table Functionality using hashtable implementation.
- [x] Integrate Symbol Table into lexer.
- [x] Add proper pattern rules to flex program.
