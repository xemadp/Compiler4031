%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../symboltable/symtab.h"
#include "../parser/translate.tab.h"

// Global symbol table
extern SymbolTable* symboltable;

// Current scope tracking
static int current_scope = 0;

// Forward declarations
static void init_symbol_table(void);
static Symbol* install_identifier(const char* name, int kind);
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
"true"          { return TRUE_KW; }
"false"         { return FALSE_KW; }

":="            { return ASSIGN_OP; }
"*"             { return MUL_OP; }
"/"             { return DIV_OP; }
"+"             { return ADD_OP; }
"-"             { return SUB_OP; }
"<"             { return LT_OP; }
"<="            { return LE_OP; }
"<>"            { return NE_OP; }
"="             { return EQ_OP; }
">="            { return GE_OP; }
">"             { return GT_OP; }
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
    yylval.real_val = value;
    
    // Install the constant in symbol table
    char* value_str = value_to_string(&value, TYPE_REAL);
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
    yylval.int_val = value;
    
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

/* Initialize the symbol table */
static void init_symbol_table(void) {
    if (!symboltable) {
        symboltable = symtab_create(101); // Start with reasonable size
        if (!symboltable) {
            fprintf(stderr, "Failed to create symbol table\n");
            exit(1);
        }
    }
}

/* Install an identifier in the symbol table */
static Symbol* install_identifier(const char* name, int kind) {
    if (!symboltable) init_symbol_table();
    
    // First check if symbol exists in current scope
    Symbol* existing = symtab_lookup(symboltable, name, current_scope);
    if (existing) return existing;
    
    // Create new symbol
    Symbol new_symbol = {
        .name = strdup(name),
        .kind = kind,
        .type = TYPE_UNKNOWN, // Type will be set during semantic analysis
        .scope = current_scope,
        .line_number = yylineno,
        .is_initialized = true
    };
    
    // Insert into symbol table
    if (symtab_insert(symboltable, &new_symbol)) {
        return symtab_lookup(symboltable, name, current_scope);
    }
    
    free((void*)new_symbol.name);
    return NULL;
}

/* Install a constant in the symbol table */
static Symbol* install_constant(const char* value, DataType type) {
    if (!symboltable) init_symbol_table();
    
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
