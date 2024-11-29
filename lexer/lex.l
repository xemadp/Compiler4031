%{
#include <stdio.h>
#include <stdlib.h>
#include "../symboltable/symtab.h"

HashTable *symboltable; // Global SymbolTable Declaration.
int row_index = 1;
FILE *output; // Output file pointer

%}


%option noyywrap


digit            [0-9]
integer          0|[1-9][0-9]*
leadingInt       [0]+{integer}
optionalFrac     (\.{digit}+)
optionalExp      ([E][+-]?{digit}+)
real             {integer}{optionalFrac}{optionalExp}|{integer}{optionalFrac}|{integer}{optionalExp}
leadingReal      [0]+{real}
id               [a-zA-Z][a-zA-Z0-9]*
wrongID          {digit}+{id}

%%              

[ \t] { /* Ignore White Space */};

\n {yylineno++;} // Increment the line number

"program"          { fprintf(output, "%s\t\t <PROGRAM_KW, ->\n", yytext);}         
"function"         { fprintf(output, "%s\t\t <FUNCTION_KW, ->\n", yytext);}         
"begin"            { fprintf(output, "%s\t\t <BEGIN_KW, ->\n", yytext);}         
"end"              { fprintf(output, "%s\t\t <END_KW, ->\n", yytext);}         
"if"               { fprintf(output, "%s\t\t <IF_KW, ->\n", yytext);}         
"while"            { fprintf(output, "%s\t\t <WHILE_KW, ->\n", yytext);}         
"do"               { fprintf(output, "%s\t\t <DO_KW, ->\n", yytext);}         
"for"              { fprintf(output, "%s\t\t <FOR_KW, ->\n", yytext);}         
"to"               { fprintf(output, "%s\t\t <TO_KW, ->\n", yytext);}         
"then"             { fprintf(output, "%s\t\t <THEN_KW, ->\n", yytext);}         
"else"             { fprintf(output, "%s\t\t <ELSE_KW, ->\n", yytext);}         
"integer"          { fprintf(output, "%s\t\t <INTEGER_KW, ->\n", yytext);}         
"real"             { fprintf(output, "%s\t\t <REAL_KW, ->\n", yytext);}         
"boolean"          { fprintf(output, "%s\t\t <BOOLEAN_KW, ->\n", yytext);}         
"return"           { fprintf(output, "%s\t\t <RETURN_KW, ->\n", yytext);}         
"and"              { fprintf(output, "%s\t\t <AND_KW, ->\n", yytext);}         
"or"               { fprintf(output, "%s\t\t <OR_KW, ->\n", yytext);}         
"true"             { fprintf(output, "%s\t\t <TRUE_KW, ->\n", yytext);}         
"false"            { fprintf(output, "%s\t\t <FALSE_KW, ->\n", yytext);}         
":="               { fprintf(output, "%s\t\t <ASSIGN_OP, ->\n", yytext);}         
"*"                { fprintf(output, "%s\t\t <MUL_OP, ->\n", yytext);}         
"/"                { fprintf(output, "%s\t\t <DIV_OP, ->\n", yytext);}         
"+"                { fprintf(output, "%s\t\t <ADD_OP, ->\n", yytext);}         
"-"                { fprintf(output, "%s\t\t <SUB_OP, ->\n", yytext);}         
"<"                { fprintf(output, "%s\t\t <LT_OP, ->\n", yytext);}         
"<="               { fprintf(output, "%s\t\t <LE_OP, ->\n", yytext);}         
"<>"               { fprintf(output, "%s\t\t <NE_OP, ->\n", yytext);}         
"="                { fprintf(output, "%s\t\t <EQ_OP, ->\n", yytext);}         
">="               { fprintf(output, "%s\t\t <GE_OP, ->\n", yytext);}         
">"                { fprintf(output, "%s\t\t <GT_OP, ->\n", yytext);}         
":"                { fprintf(output, "%s\t\t <COLON, ->\n", yytext);}         
";"                { fprintf(output, "%s\t\t <SEMICOLON, ->\n", yytext);}         
","                { fprintf(output, "%s\t\t <COMMA, ->\n", yytext);}         
"("                { fprintf(output, "%s\t\t <LEFT_PA, ->\n", yytext);}         
")"                { fprintf(output, "%s\t\t <RIGHT_PA, ->\n", yytext);}         

{leadingReal} {
    fprintf(output, "ILLEGAL LEXEME:\t %s\n", yytext);
}

{real} {
    Symbol s = search(symboltable, yytext);
    int row;
    if (s.key != NULL) { // getToken();
        row = s.row;
    } else { // installReal() : Add new token to the symbol table
        row = row_index;
        insert(symboltable, yytext, NULL, SYMBOL_TYPE_CONSTANT, yylineno, row);
        row_index++;
    }
    fprintf(output, "%s\t\t <REAL_NUMBER, %d>\n", yytext, row);
}

{leadingInt} {
    fprintf(output, "ILLEGAL LEXEME:\t %s\n", yytext);
}


{integer} {
    Symbol s = search(symboltable, yytext);
    int row;
    if (s.key != NULL) { // getToken();
        row = s.row;
    } else { // installInteger() : Add new token to the symbol table
        row = row_index;
        insert(symboltable, yytext, NULL, SYMBOL_TYPE_CONSTANT, yylineno, row);
        row_index++;
    }
    fprintf(output, "%s\t\t <INTEGER_NUMBER, %d>\n", yytext, row);
}

{wrongID} {
    fprintf(output, "ILLEGAL LEXEME:\t %s\n", yytext);
}

{id} { 
    Symbol s = search(symboltable, yytext);
    int row;
    if (s.key != NULL) {
        row = s.row; // getToken() : Existing token in the symbol table
    } else {
        row = row_index; // installID() : Add new token to the symbol table
        insert(symboltable, yytext, NULL, SYMBOL_TYPE_IDENTIFIER, yylineno, row);
        row_index++;
    }

    fprintf(output, "%s\t\t <IDENTIFIER, %d>\n", yytext, row);
}



. {
    fprintf(output, "ILLEGAL LEXEME: %s\n", yytext);
}

%%

int main(void) {
    symboltable = create_table(7459);

    FILE *input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");

    if (!output) {
        fprintf(stderr, "Error creating output file\n");
        return 1;
    }

    fprintf(output, "Emad Pourhassani <40116623, ->\nAfarin Akhoundi <40115283, ->\n----------------------------------------------\n");

    if (input) {
        yyin = input;  // Set yyin to read from a file
        yylex();       // Start the lexical analysis
        fclose(input); // Close the file after use
    } else {
        fprintf(stderr, "Error opening input file\n");
    }

    free_table(symboltable);
    fclose(output); // Close the output file
    return 0;
}
