%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../symboltable/symtab.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
void yyerror(const char* s);

/* Global Variables */
SymbolTable* symboltable;  
FILE* output;
int current_scope = 0;    // Global scope starts at 0
DataType current_type;    // Tracks current declaration type

/* Symbol Table Management Functions */
void init_symbol_table() {
    symboltable = symtab_create(1000);  // Create with reasonable initial size
    if (!symboltable) {
        fprintf(stderr, "Failed to create symbol table\n");
        exit(1);
    }
}

void declare_variable(const char* name) {
    Symbol* existing = symtab_lookup(symboltable, name, current_scope);
    if (existing) {
        fprintf(stderr, "Error at line %d: Redeclaration of variable '%s' in current scope\n",
                yylineno, name);
        return;
    }

    Symbol new_symbol = {
        .name = strdup(name),
        .type = current_type,
        .kind = SYMBOL_VARIABLE,
        .scope = current_scope,
        .line_number = yylineno,
        .is_initialized = false
    };
    
    if (!symtab_insert(symboltable, &new_symbol)) {
        fprintf(stderr, "Error at line %d: Failed to insert variable '%s' into symbol table\n",
                yylineno, name);
        free(new_symbol.name);
    }
}

void enter_scope() {
    current_scope++;
}

void exit_scope() {
    current_scope--;
}

void print_reduction(int rule_num, const char* rule_text) {
    fprintf(output, "%d\t%s\n", rule_num, rule_text);
}

%}

/* Token definitions */
%union {
    int int_val;
    double real_val;
    char* id_name;
}

%token PROGRAM_KW FUNCTION_KW BEGIN_KW END_KW
%token IF_KW WHILE_KW DO_KW FOR_KW TO_KW THEN_KW ELSE_KW
%token INTEGER_KW REAL_KW BOOLEAN_KW
%token RETURN_KW AND_KW OR_KW
%token TRUE_KW FALSE_KW
%token ASSIGN_OP
%token MUL_OP DIV_OP ADD_OP SUB_OP
%token LT_OP LE_OP NE_OP EQ_OP GE_OP GT_OP
%token COLON SEMICOLON COMMA
%token LEFT_PA RIGHT_PA
%token <int_val> INTEGER_NUMBER
%token <real_val> REAL_NUMBER
%token <id_name> IDENTIFIER

/* Operator precedence */
%left OR_KW
%left AND_KW
%nonassoc LT_OP LE_OP EQ_OP NE_OP GE_OP GT_OP
%left ADD_OP SUB_OP
%left MUL_OP DIV_OP

%start start

%%

start
    : PROGRAM_KW
    IDENTIFIER
    {
    declare_variable($2);
}
    SEMICOLON
    decList funcList block
    {
        print_reduction(yyn-2, "start -> PROGRAM_KW IDENTIFIER SEMICOLON decList funcList block");
    }
    ;

decList
    : decs
    {
        print_reduction(yyn-2, "decList -> decs");
    }
    | decs decList
    {
        print_reduction(yyn-2, "decList -> decs decList");
    }
    ;

decs
    : type varList SEMICOLON
    {
        print_reduction(yyn-2, "decs -> type varList SEMICOLON");
    }
    | /* empty */
    {
        print_reduction(yyn-2, "decs -> epsilon");
    }
    ;

type
    : INTEGER_KW
    {
        current_type = TYPE_INT;
        print_reduction(yyn-2, "type -> INTEGER_KW");
    }
    | REAL_KW
    {
        current_type = TYPE_REAL;
        print_reduction(yyn-2, "type -> REAL_KW");
    }
    | BOOLEAN_KW
    {
        current_type = TYPE_BOOL;
        print_reduction(yyn-2, "type -> BOOLEAN_KW");
    }
    ;

varList
    : IDENTIFIER
    {
        declare_variable($1);
        print_reduction(yyn-2, "varList -> IDENTIFIER");
    }
    | varList COMMA IDENTIFIER
    {
        declare_variable($3);
        print_reduction(yyn-2, "varList -> varList COMMA IDENTIFIER");
    }
    ;

/* Rest of the grammar rules remain the same, but without symbol table actions */
funcList
    : funcList funcDec
    {
        print_reduction(yyn-2, "funcList -> funcList funcDec");
    }
    |   /* empty */
    {
        print_reduction(yyn-2, "funcList -> epsilon");
    }
    ;

funcDec
    : FUNCTION_KW IDENTIFIER parameters COLON type decList block
    {
        print_reduction(yyn-2, "funcDec -> FUNCTION_KW IDENTIFIER parameters COLON type decList block");
    }
    ;

parameters
    : LEFT_PA decList RIGHT_PA
    {
        print_reduction(yyn-2, "parameters -> LEFT_PA decList RIGHT_PA");
    }
    ;

block
    : BEGIN_KW stmtList END_KW
    {
        print_reduction(yyn-2, "block -> BEGIN_KW stmtList END_KW");
    }
    ;

stmtList
    : stmt
    {
        print_reduction(yyn-2, "stmtList -> stmt");
    }
    | stmtList stmt
    {
        print_reduction(yyn-2, "stmtList -> stmtList stmt");
    }
    ;

stmt
    : IDENTIFIER ASSIGN_OP expr SEMICOLON
    {
        print_reduction(yyn-2, "stmt -> IDENTIFIER ASSIGN_OP expr SEMICOLON");
    }
    | IF_KW expr THEN_KW stmt
    {
        print_reduction(yyn-2, "stmt -> IF_KW expr THEN_KW stmt");
    }
    | IF_KW expr THEN_KW stmt ELSE_KW stmt
    {
        print_reduction(yyn-2, "stmt -> IF_KW expr THEN_KW stmt ELSE_KW stmt");
    }
    | WHILE_KW expr DO_KW stmt
    {
        print_reduction(yyn-2, "stmt -> WHILE_KW expr DO_KW stmt");
    }
    | FOR_KW IDENTIFIER ASSIGN_OP expr TO_KW expr DO_KW stmt
    {
        print_reduction(yyn-2, "stmt -> FOR IDENTIFIER ASSIGN_OP expr TO_KW expr DO_KW stmt");
    }
    | RETURN_KW expr SEMICOLON
    {
        print_reduction(yyn-2, "stmt -> RETURN_KW expr SEMICOLON");
    }
    | block
    {
        print_reduction(yyn-2, "stmt -> block");
    }
    ;

expr
    : expr AND_KW expr
    {
        print_reduction(yyn-2, "expr -> expr AND_KW expr");
    }
    | expr OR_KW expr
    {
        print_reduction(yyn-2, "expr -> expr OR_KW expr");
    }
    | expr MUL_OP expr
    {
        print_reduction(yyn-2, "expr -> expr MUL_OP expr");
    }
    | expr DIV_OP expr
    {
        print_reduction(yyn-2, "expr -> expr DIV_OP expr");
    }
    | expr ADD_OP expr
    {
        print_reduction(yyn-2, "expr -> expr ADD_OP expr");
    }
    | expr SUB_OP expr
    {
        print_reduction(yyn-2, "expr -> expr SUB_OP expr");
    }
    | expr relop expr
    {
        print_reduction(yyn-2, "expr -> expr relop expr");
    }
    | LEFT_PA expr RIGHT_PA
    {
        print_reduction(yyn-2, "expr -> LEFT_PA expr RIGHT_PA");
    }
    | INTEGER_NUMBER
    {
        print_reduction(yyn-2, "expr -> INTEGER_NUMBER");
    }
    | REAL_NUMBER
    {
        print_reduction(yyn-2, "expr -> REAL_NUMBER");
    }
    | TRUE_KW
    {
        print_reduction(yyn-2, "expr -> TRUE_KW");
    }
    | FALSE_KW
    {
        print_reduction(yyn-2, "expr -> FALSE_KW");
    }
    | IDENTIFIER LEFT_PA actualParamList RIGHT_PA
    {
        print_reduction(yyn-2, "expr -> IDENTIFIER LEFT_PA actualParamList RIGHT_PA");
    }
    | IDENTIFIER
    {
        print_reduction(yyn-2, "expr -> IDENTIFIER");
    }
    ;

actualParamList
    : expr
    {
        print_reduction(yyn-2, "actualParamList -> expr");
    }
    | actualParamList COMMA expr
    {
        print_reduction(yyn-2, "actualParamList -> actualParamList COMMA expr");
    }
    |   /* empty */
    {
        print_reduction(yyn-2, "actualParamList -> epsilon");
    }
    ;

relop
    : LT_OP
    {
        print_reduction(yyn-2, "relop -> LT_OP");
    }
    | LE_OP
    {
        print_reduction(yyn-2, "relop -> LE_OP");
    }
    | EQ_OP
    {
        print_reduction(yyn-2, "relop -> EQ_OP");
    }
    | NE_OP
    {
        print_reduction(yyn-2, "relop -> NE_OP");
    }
    | GE_OP
    {
        print_reduction(yyn-2, "relop -> GE_OP");
    }
    | GT_OP
    {
        print_reduction(yyn-2, "relop -> GT_OP");
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error at line %d: %s near token '%s'\n", 
            yylineno, s, yytext);
}

int main(void) {
    // Initialize symbol table
    init_symbol_table();
    
    // Open input and output files
    yyin = fopen("input.txt", "r");
    output = fopen("output.txt", "w");
    
    if (!yyin) {
        fprintf(stderr, "Error: Cannot open input file 'input.txt'\n");
        symtab_destroy(symboltable);
        return 1;
    }
    
    if (!output) {
        fprintf(stderr, "Error: Cannot create output file 'output.txt'\n");
        fclose(yyin);
        symtab_destroy(symboltable);
        return 1;
    }
    
    // Write NAME AND STUDENT NUMBER 
    fprintf(output, "Emad Pourhassani -> 40116623\n");
    fprintf(output, "Afarin Akhoundi  -> 40115283\n");
    fprintf(output, "----------------------------\n");
    
    // Parse input
    int result = yyparse();
    
    // Print final symbol table
    symtab_print(symboltable);
    
    // Cleanup
    symtab_destroy(symboltable);
    fclose(yyin);
    fclose(output);
    
    return result;
}
