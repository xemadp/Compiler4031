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
FILE* TACoutput;
#define MAX_SCOPES 100 // Maximum number of nested scopes
DataType current_type;    // Tracks current declaration type

int label_count = 0;
char label_buffer[20];

/* Function to generate new label */
char* new_label() {
    snprintf(label_buffer, sizeof(label_buffer), "L%d", label_count++);
    return strdup(label_buffer);
}

/* Symbol Table Management Functions */
void init_symbol_table() {
    symboltable = symtab_create(1000);  // Create with reasonable initial size
    if (!symboltable) {
        fprintf(stderr, "Failed to create symbol table\n");
        exit(1);
    }
}


int scope_stack[MAX_SCOPES]; // Stack to hold scope IDs
int top = -1;                // Index of the top of the stack
int next_scope_id = 1;       // Unique ID generator

// Push a scope ID onto the stack
void push_scope(int scope_id) {
    if (top >= MAX_SCOPES - 1) {
        fprintf(stderr, "Scope stack overflow\n");
        exit(EXIT_FAILURE);
    }
    scope_stack[++top] = scope_id;
}

// Pop the top scope ID off the stack
void pop_scope() {
    if (top < 0) {
        fprintf(stderr, "Scope stack underflow\n");
        exit(EXIT_FAILURE);
    }
    top--;
}

// Get the current scope ID (top of the stack)
int current_scope() {
    if (top < 0) {
        fprintf(stderr, "No active scope\n");
        exit(EXIT_FAILURE);
    }
    return scope_stack[top];
}

// Enter a new scope
void enter_scope() {
    int new_scope_id = next_scope_id++; // Generate a unique ID
    push_scope(new_scope_id);          // Push the new scope onto the stack
    //printf("Entered scope %d\n", new_scope_id);
}

// Exit the current scope
void exit_scope() {
    //printf("Exiting scope %d\n", current_scope());
    pop_scope(); // Pop the current scope from the stack
}

// function declaration, basically only saves the identifier that is the name of a function.
// Program name is considered a function name.
void declare_function(const char* name) {
    Symbol* existing = symtab_lookup(symboltable, name, current_scope());
    if (existing && existing->kind==SYMBOL_FUNCTION) {
        fprintf(stderr, "Error at line %d: Redeclaration of function '%s' in current scope\n",
                yylineno, name);
        return;
    }

    Symbol new_symbol = {
        .name = strdup(name),
        .type = current_type,
        .kind = SYMBOL_FUNCTION,
        .scope = current_scope(),
        .line_number = yylineno,
        .is_initialized = false
    };
    
    if (!symtab_insert(symboltable, &new_symbol)) {
        fprintf(stderr, "Error at line %d: Failed to insert function name '%s' into symbol table\n",
                yylineno, name);
        free(new_symbol.name);
    }
}

void declare_variable(const char* name) {
    Symbol* existing = symtab_lookup(symboltable, name, current_scope());
    if (existing) {
        fprintf(stderr, "Error at line %d: Redeclaration of variable '%s' in current scope\n",
                yylineno, name);
        return;
    }
    Symbol new_symbol = {
        .name = strdup(name),
        .type = current_type,
        .kind = SYMBOL_VARIABLE,
        .scope = current_scope(),
        .line_number = yylineno,
        .is_initialized = false
    };
    
    if (!symtab_insert(symboltable, &new_symbol)) {
        fprintf(stderr, "Error at line %d: Failed to insert variable '%s' into symbol table\n",
                yylineno, name);
        free(new_symbol.name);
    }
}

// Print Current reduction
void print_reduction(int rule_num, const char* rule_text) {
    fprintf(output, "%d\t%s\n", rule_num, rule_text);
}

%}

/* Token definitions */
%union {
    int int_val;
    double real_val;
    char* id_name;
    LabelPair label_pair;
}

%token PROGRAM_KW FUNCTION_KW BEGIN_KW END_KW
%token IF_KW WHILE_KW DO_KW FOR_KW TO_KW THEN_KW ELSE_KW
%token INTEGER_KW REAL_KW BOOLEAN_KW
%token RETURN_KW AND_KW OR_KW
%token ASSIGN_OP
%token MUL_OP DIV_OP ADD_OP SUB_OP
%token LT_OP LE_OP NE_OP EQ_OP GE_OP GT_OP
%token COLON SEMICOLON COMMA
%token LEFT_PA RIGHT_PA
%token <id_name> INTEGER_NUMBER
%token <id_name> REAL_NUMBER
%token <id_name> TRUE_KW FALSE_KW
%token <id_name> IDENTIFIER

%type <id_name> expr relop
%type <id_name> stmt if_body

/* Operator precedence */
%left OR_KW
%left AND_KW
%nonassoc LT_OP LE_OP EQ_OP NE_OP GE_OP GT_OP
%left ADD_OP SUB_OP
%left MUL_OP DIV_OP

%start start

%%

start
    : PROGRAM_KW IDENTIFIER
    {
    enter_scope();
    declare_function($2);
    }
    SEMICOLON
    decList funcList block
    {
        print_reduction(yyn-2, "start -> PROGRAM_KW IDENTIFIER SEMICOLON decList funcList block");
        exit_scope();
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
        declare_function($2);
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
        Quadruple quad = create_quadruple(symboltable,":=",$3,"", $1);
        add_quadruple(symboltable,quad);

        //printf("%s := %s\n", $1, $3);
        print_reduction(yyn-2, "stmt -> IDENTIFIER ASSIGN_OP expr SEMICOLON");
    }
    | IF_KW expr
    {
        char* TRUE_label = new_label();   // if cond is true
        char* NEXT_label = new_label();  // code after if stmt
        char* ELSE_label = new_label(); // if cond is not true hence else is true
        
        // Create Quadruples
        Quadruple ifquad = create_quadruple(symboltable,"IF",$2,"", TRUE_label);
        //printf("if (%s) goto %s\n", $2, TRUE_label);
        add_quadruple(symboltable, ifquad);

        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", NEXT_label, "", "");
        //printf("goto %s\n", NEXT_label);
        add_quadruple(symboltable, gotoquad);

        Quadruple labelquad = create_quadruple(symboltable, "LABEL", TRUE_label, "", "");
        //printf("%s : \n", TRUE_label);
        add_quadruple(symboltable, labelquad);
        
        $<label_pair>$.label1 = NEXT_label;
        $<label_pair>$.label2 = ELSE_label;
        $<label_pair>$.label3 = TRUE_label;
        
    }
    THEN_KW if_body
      {
      if (strcmp($5,"hasElse")==0){
          printf("hasElse Check Passed!!!!\n");
          Quadruple labelquad = create_quadruple(symboltable, "LABEL",$<label_pair>3.label2, "", "");
          add_quadruple(symboltable, labelquad);
          //printf("%s : \n", NEXT_label);

          // BACKPATCH THE IF-ELSE LABELS!
          // Using the  backpatch function from symtab.c here.
         
          BackPatchLabels(symboltable, $<label_pair>3.label2, $<label_pair>3.label1);
      }
      else
      {
          printf("hasElse Check ---NOT--- Passed!!!!\n");
         Quadruple labelquad = create_quadruple(symboltable, "LABEL",$<label_pair>3.label1, "", "");
         //printf("%s : \n", NEXT_label);
         add_quadruple(symboltable, labelquad);
         //printf("%s : \n", $<label_pair>3.label1);
        
      }
      print_reduction(yyn-2, "stmt -> IF_KW expr THEN_KW stmt ELSE_KW stmt");
    }
    | WHILE_KW expr
    {
        char* WHILE_label = new_label(); // before loop starts
        char* TRUE_label = new_label();   // if loop cond is true
        char* FALSE_label = new_label();  // if loop cond is false
        $<label_pair>$.label1 = WHILE_label;
        $<label_pair>$.label2 = FALSE_label;

        Quadruple labelquad = create_quadruple(symboltable, "LABEL", WHILE_label, "", "");
        add_quadruple(symboltable, labelquad);
        //printf("%s : \n", WHILE_label);
        Quadruple ifquad = create_quadruple(symboltable, "IF", $2, "", TRUE_label);
        add_quadruple(symboltable, ifquad);
        //printf("if (%s) goto %s\n", $2, TRUE_label);
        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", FALSE_label, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", FALSE_label);
        Quadruple tlabelquad = create_quadruple(symboltable, "LABEL", TRUE_label, "", "");
        add_quadruple(symboltable, tlabelquad);
        //printf("%s : \n", TRUE_label);
    }
    DO_KW stmt
    {   
        Quadruple gotoquad = create_quadruple(symboltable, "GOTO",$<label_pair>3.label1, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", $<label_pair>3.label1);
        Quadruple flabelquad = create_quadruple(symboltable, "LABEL", $<label_pair>3.label2, "", "");
        add_quadruple(symboltable, flabelquad);
        //printf("%s : \n", $<label_pair>3.label2);
        print_reduction(yyn-2, "stmt -> WHILE_KW expr DO_KW stmt");
    }
    | FOR_KW IDENTIFIER ASSIGN_OP expr TO_KW expr
    {
        enter_scope();
        declare_variable($2);
        
        Quadruple quad = create_quadruple(symboltable,":=",$4,"", $2);
        add_quadruple(symboltable, quad);
        //printf("%s := %s\n", $2, $4);

        char* temp = create_temp(symboltable);

        // Add some quad here first for later
        Quadruple forCondQuad = create_quadruple(symboltable,"<=",$2,$6, temp);
        add_quadruple(symboltable, forCondQuad);
        //printf("%s := %s <= %s\n", temp, $2, $6);
        
        char* CONDITION_label = new_label(); // Condition of For loop
        char* BODY_label = new_label(); // BODY of For loop
        char* NEXT_label = new_label(); // Code after For loop
        
        Quadruple clabelquad = create_quadruple(symboltable, "LABEL", CONDITION_label, "", "");
        add_quadruple(symboltable, clabelquad);
        //printf("%s : \n", CONDITION_label);

        Quadruple ifquad = create_quadruple(symboltable, "IF", temp, "", BODY_label);
        add_quadruple(symboltable, ifquad);
        //printf("if (%s) goto %s\n", temp, BODY_label);

        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", NEXT_label, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", NEXT_label);

        Quadruple blabelquad = create_quadruple(symboltable, "LABEL", BODY_label, "", "");
        add_quadruple(symboltable, blabelquad);
        //printf("%s : \n", BODY_label);

        $<label_pair>$.label1 = CONDITION_label;
        $<label_pair>$.label2 = strdup($2);
        $<label_pair>$.label3 = NEXT_label;
       

    }
     DO_KW stmt
    {
        // for x:= 1 to n do ... -> x := x + 1 in end of for loop before going back to check the for cond.
        Quadruple forIncrementQuad = create_quadruple(symboltable,"+", $<label_pair>7.label2, "1", $<label_pair>7.label2);
        add_quadruple(symboltable, forIncrementQuad);
        //printf("%s := %s + 1 \n", $<label_pair>7.label2, $<label_pair>7.label2);

        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", $<label_pair>7.label1, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", $<label_pair>7.label1);

        Quadruple nlabelquad = create_quadruple(symboltable, "LABEL",$<label_pair>7.label3, "", "");
        add_quadruple(symboltable, nlabelquad);
        //printf("%s : \n", $<label_pair>7.label3);

        exit_scope();
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

if_body
    : stmt
    {
    // Do nothing.
    }
    | stmt ELSE_KW
    {
        printf("---------SEEEN ELSE IN INPUT------------\n");
        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", "UNFILLED ELSE", "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto  ---- <- NEXT LABEL FOR IF-ELSE STMT\n");

        Quadruple elabelquad = create_quadruple(symboltable, "LABEL","UNFILLED NEXT", "", "");
        add_quadruple(symboltable, elabelquad);
        //printf("LABEL ---- <- ELSE LABEL FOR IF-ELSE STMT \n");
    }
    stmt
    {
    $$ = "hasElse";
    // Do nothing.
    }

expr
    : expr AND_KW expr
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"&&",$1,$3, temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s && %s\n", $$, $1, $3);
            
        $$ = temp;
        print_reduction(yyn-2, "expr -> expr AND_KW expr");
    }
    | expr OR_KW expr
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"||",$1,$3, temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s || %s\n", $$, $1, $3);
            
        $$ = temp;
        print_reduction(yyn-2, "expr -> expr OR_KW expr");
    }
    | expr MUL_OP expr
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"*",$1,$3, temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s * %s\n", $$, $1, $3);
            
        $$ = temp;
        //printf("expr_1 -> expr_2 * expr_3 was reduced. where expr_1 is %s\n", temp);
        print_reduction(yyn-2, "expr -> expr MUL_OP expr");
    }
    | expr DIV_OP expr
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"/",$1, $3, temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s / %s\n", $$, $1, $3);
            
        $$ = temp;
        print_reduction(yyn-2, "expr -> expr DIV_OP expr");
    }
    | expr ADD_OP expr
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"+",$1,$3, temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s + %s\n", $$, $1, $3);
            
        $$ = temp;
        //printf("expr_1 -> expr_2 + expr_3 was reduced. where expr_1 is %s\n", temp);
        print_reduction(yyn-2, "expr -> expr ADD_OP expr");
    }
    | expr SUB_OP expr
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"-",$1,$3, temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s - %s\n", $$, $1, $3);
            
        $$ = temp;
        print_reduction(yyn-2, "expr -> expr SUB_OP expr");
    }
    | expr relop expr
    {   
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,$2,$1,$3, temp);
        add_quadruple(symboltable, quad);
            
        $$ = temp;
        //printf("%s := %s %s %s\n", $$, $1, $2, $3);
        print_reduction(yyn-2, "expr -> expr relop expr");
    }
    | LEFT_PA expr RIGHT_PA
    {
        print_reduction(yyn-2, "expr -> LEFT_PA expr RIGHT_PA");
        $$ = $2;
    }
    | INTEGER_NUMBER
    { 
        print_reduction(yyn-2, "expr -> INTEGER_NUMBER");
        $$ = $1;
    }
    | REAL_NUMBER
    {
        print_reduction(yyn-2, "expr -> REAL_NUMBER");
        $$ = $1;
    }
    | TRUE_KW
    {
        print_reduction(yyn-2, "expr -> TRUE_KW");
        $$ = $1;
    }
    | FALSE_KW
    {
        print_reduction(yyn-2, "expr -> FALSE_KW");
        $$ = $1;
    }
    | IDENTIFIER LEFT_PA actualParamList RIGHT_PA
    {
        print_reduction(yyn-2, "expr -> IDENTIFIER LEFT_PA actualParamList RIGHT_PA");
    }
    | IDENTIFIER
    {
        print_reduction(yyn-2, "expr -> IDENTIFIER");
        $$ = $1;
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
    output = fopen("parser-output.txt", "w");
    
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

    TACoutput = fopen("output.txt", "w");
    fprintf(TACoutput, "Emad Pourhassani := 40116623\n");
    fprintf(TACoutput, "Afarin Akhoundi  := 40115283\n");
    fprintf(TACoutput, "----------------------------\n");
    
    // Print final symbol table and quadruples to output.txt
    // generate TAC as well.
    symtab_fprint(symboltable, TACoutput);

    fprintf(TACoutput, "\nThree Address Codes:\n----------------------------\n");
    iterate_quadruples(symboltable, fprint_TAC, TACoutput);
    
    // Cleanup
    symtab_destroy(symboltable);
    fclose(yyin);
    fclose(output);
    
    return result;
}
