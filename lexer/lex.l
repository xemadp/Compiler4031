%{
#include <stdio.h>
#include <stdlib.h>
#include "../symboltable/symtab.h"

//HashTable *symboltable; // Global SymbolTable Declaration.
//int row_index = 1;
%}

%%

.                 { printf("Unknown character\n"); }

%%

int main(void) {
    yylex();
    //symboltable = create_table(313);

    //FILE *input = fopen("input.txt", "r");
    //if (input) {
    //    yyin = input;  // Set yyin to read from a file
    //    yylex();       // Start the lexical analysis
    //    fclose(input); // Close the file after use
    //} else {
    //    fprintf(stderr, "Error opening file\n");
    // }

    //free_table(symboltable);
    return 0;
}
