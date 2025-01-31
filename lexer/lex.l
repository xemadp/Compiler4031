%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../symboltable/symtab.h"
#include "../parser/translate.tab.h"

// Global symbol table
extern SymbolTable* symboltable;

// Forward declarations
static Symbol* install_constant(const char* value, DataType type);

// Helper function to convert values to strings
static char* value_to_string(const void* value, DataType type);

extern FILE* output;
%}

%option noyywrap
%option yylineno

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

[ \t]           { /* Ignore White Space */ }
\n              { /* Track line numbers */ }

"program"       { return PROGRAM_KW; }
"function"      { return FUNCTION_KW; }
"begin"         { return BEGIN_KW; }
"end"           { return END_KW; }

"if"            { return IF_KW; }
"while"         { return WHILE_KW; }
"do"            { return DO_KW; }
"for"           { return FOR_KW; }
"to"            { return TO_KW; }
"then"          { return THEN_KW; }
"else"          { return ELSE_KW; }
"integer"       { return INTEGER_KW; }
"real"          { return REAL_KW; }
"boolean"       { return BOOLEAN_KW; }
"return"        { return RETURN_KW; }
"and"           { return AND_KW; }
"or"            { return OR_KW; }
"true"          { yylval.id_name = "true";   return TRUE_KW; }
"false"         { yylval.id_name = "false"; return FALSE_KW; }

":="            { return ASSIGN_OP; }
"*"             { return MUL_OP; }
"/"             { return DIV_OP; }
"+"             { return ADD_OP; }
"-"             { return SUB_OP; }
"<"             { yylval.id_name = "<"; return LT_OP; }
"<="            { yylval.id_name = "<="; return LE_OP; }
"<>"            { yylval.id_name = "<>"; return NE_OP; }
"="             { yylval.id_name = "="; return EQ_OP; }
">="            { yylval.id_name = ">="; return GE_OP; }
">"             { yylval.id_name = ">"; return GT_OP; }
":"             { return COLON; }
";"             { return SEMICOLON; }
","             { return COMMA; }
"("             { return LEFT_PA; }
")"             { return RIGHT_PA; }

{leadingReal}   { 
    fprintf(output, "Error at line %d: Invalid real number with leading zeros: %s\n", 
            yylineno, yytext);
}

{real}          {
    // Convert the real number to a string for symbol table
    double value = atof(yytext);
    // Install the constant in symbol table
    char* value_str = value_to_string(&value, TYPE_REAL);

    yylval.id_name = strdup(value_str);
    install_constant(value_str, TYPE_REAL);
    free(value_str);
    
    return REAL_NUMBER;
}

{leadingInt}    {
    fprintf(output, "Error at line %d: Invalid integer with leading zeros: %s\n", 
            yylineno, yytext);
}

{integer}       {
    // Convert the integer to a string for symbol table
    int value = atoi(yytext);
    yylval.id_name = strdup(yytext);
    
    // Install the constant in symbol table
    char* value_str = value_to_string(&value, TYPE_INT);
    install_constant(value_str, TYPE_INT);
    free(value_str);
    
    return INTEGER_NUMBER;
}

{wrongID}       {
    fprintf(output, "Error at line %d: Invalid identifier (starts with digit): %s\n", 
            yylineno, yytext);
}

{id}            {
    yylval.id_name = strdup(yytext);
    return IDENTIFIER;
}

.               {
    fprintf(output, "Error at line %d: Illegal character: %s\n", 
            yylineno, yytext);
}

%%

/* Helper function to convert values to strings */
static char* value_to_string(const void* value, DataType type) {
    char buffer[64];  // Large enough for most numeric strings
    
    switch (type) {
        case TYPE_INT:
            snprintf(buffer, sizeof(buffer), "%d", *(const int*)value);
            break;
        case TYPE_REAL:
            snprintf(buffer, sizeof(buffer), "%.6g", *(const double*)value);
            break;
        default:
            return strdup("unknown");
    }
    
    return strdup(buffer);
}

/* Install a constant in the symbol table */
static Symbol* install_constant(const char* value, DataType type) {
    
    // Constants are stored in global scope (0)
    Symbol* existing = symtab_lookup(symboltable, value, 0);
    if (existing) return existing;
    
    // Create new symbol for constant
    Symbol new_symbol = {
        .name = strdup(value),
        .kind = SYMBOL_CONSTANT,
        .type = type,
        .scope = 0,  // Constants are always in global scope
        .line_number = yylineno,
        .is_initialized = true  // Constants are always initialized
    };
    
    // Insert into symbol table
    if (symtab_insert(symboltable, &new_symbol)) {
        return symtab_lookup(symboltable, value, 0);
    }
    
    free((void*)new_symbol.name);
    return NULL;
}
