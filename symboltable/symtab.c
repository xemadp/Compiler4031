#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* Constants for hash table management */
#define INITIAL_SIZE 101        // Prime number for better distribution
#define MAX_LOAD_FACTOR 0.7     // Resize when table is 70% full
#define GROWTH_FACTOR 2         // Double size when resizing ,  |H| -> 2|H|

/* Private function declarations */
static unsigned int hash_function(const char *str, int scope_id, int table_size);
static bool symtab_resize(SymbolTable *symtab);



/* ------------------------------------------- Symbol Table Implementation Functions ------------------------------------------- */
/* Hash function */
static unsigned int hash_function(const char *str, int scope_id, int table_size) {
    unsigned int hash = 0;
    
    // Hash the string
    while (*str) {
        hash += *str++;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    
    // Incorporate scope information
    hash += scope_id;
    hash += (hash << 10); // performing consistent shifts and adding to hash value.
    hash ^= (hash >> 6);
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return hash % table_size;
}

/* Create new symbol table */
SymbolTable* symtab_create(int initial_size) {
    SymbolTable *symtab = malloc(sizeof(SymbolTable)); // Allocate enough memory for the symbol table.
    if (!symtab) return NULL;
    
    symtab->table = calloc(initial_size, sizeof(Symbol));
    if (!symtab->table) {
        free(symtab);
        return NULL;
    }
    
    symtab->size = initial_size;
    symtab->count = 0;
    symtab->load_factor = MAX_LOAD_FACTOR;
    symtab->current_temp_index = 0;
    
    // initialize quad list
    symtab->quad_head = NULL;
    symtab->quad_tail = NULL;
    symtab->quad_count = 0;

    return symtab;
}

/* Resize the hash table when it gets too full */
static bool symtab_resize(SymbolTable *symtab) {
    int new_size = symtab->size * GROWTH_FACTOR;
    Symbol *new_table = calloc(new_size, sizeof(Symbol));
    if (!new_table) return false;
    
    // Rehash all existing entries
    for (int i = 0; i < symtab->size; i++) {
        if (symtab->table[i].is_occupied && !symtab->table[i].is_deleted) {
            unsigned int index = hash_function(symtab->table[i].name, 
                                            symtab->table[i].scope, 
                                            new_size); // make sure it is occupied and it has not been marked for deletion.
            
            // Linear probe for new position
            while (new_table[index].is_occupied) {
                index = (index + 1) % new_size;
            }
            
            new_table[index] = symtab->table[i];
        }
    }
    
    free(symtab->table);
    symtab->table = new_table;
    symtab->size = new_size;
    
    return true;
}

/* Insert a symbol into the table */
bool symtab_insert(SymbolTable *symtab, const Symbol *symbol) {
    // Check load factor and resize if necessary
    if ((float)symtab->count / symtab->size >= symtab->load_factor) {
        if (!symtab_resize(symtab)) return false;
    }
    
    unsigned int index = hash_function(symbol->name, symbol->scope, symtab->size);
    
    // Linear probe for empty slot or tombstone
    while (symtab->table[index].is_occupied && !symtab->table[index].is_deleted) { // move untill you find an occupied cell that wants to be deleted/ or a non empty cell.
        // Check for duplicate in same scope, we don't need to have multiple cells for a symbol in one scope.
        if (strcmp(symtab->table[index].name, symbol->name) == 0 &&
            symtab->table[index].scope == symbol->scope) {
            return false;  // Symbol already exists in this scope
        }
        index = (index + 1) % symtab->size;
    }
    
    // Deep copy the symbol
    symtab->table[index] = *symbol;
    symtab->table[index].name = strdup(symbol->name);
    symtab->table[index].is_occupied = true;
    symtab->table[index].is_deleted = false;
    symtab->count++;
    
    return true;
}

/* Look up a symbol in the table */
Symbol* symtab_lookup(SymbolTable *symtab, const char *name, int scope_id) {
    unsigned int index = hash_function(name, scope_id, symtab->size);
    
    // Linear probe until we find the symbol or an empty slot
    while (symtab->table[index].is_occupied) {
        if (!symtab->table[index].is_deleted &&
            strcmp(symtab->table[index].name, name) == 0 &&
            symtab->table[index].scope == scope_id) {
            return &symtab->table[index];
        }
        index = (index + 1) % symtab->size;
    }
    
    return NULL;  // Symbol not found
}

/* Delete a symbol from the table */
void symtab_delete(SymbolTable *symtab, const char *name, int scope_id) {
    unsigned int index = hash_function(name, scope_id, symtab->size);
    
    while (symtab->table[index].is_occupied) {
        if (!symtab->table[index].is_deleted &&
            strcmp(symtab->table[index].name, name) == 0 &&
            symtab->table[index].scope == scope_id) {
            
            free(symtab->table[index].name);
            symtab->table[index].is_deleted = true;
            symtab->count--;
            return;
        }
        index = (index + 1) % symtab->size;
    }
}

/* Clean up the symbol table */
void symtab_destroy(SymbolTable *symtab) {
    if (!symtab) return;
    
    for (int i = 0; i < symtab->size; i++) {
        if (symtab->table[i].is_occupied && !symtab->table[i].is_deleted) {
            free(symtab->table[i].name);
        }
    }
     // Free quadruple list
    QuadrupleNode* current = symtab->quad_head;
    while (current != NULL) {
        QuadrupleNode* next = current->next;
        
        // Free the strings inside the quadruple
        free(current->quad.op);
        free(current->quad.arg1);
        free(current->quad.arg2);
        free(current->quad.result);
        
        // Free the node itself
        free(current);
        current = next;
    }

    free(symtab->table);
    free(symtab);
}

/* ----------------------------------------------------------------------------------------------------------------------------- */


/* ------------------------------------------- Intermediate Representation Functions ------------------------------------------- */



// Create quadruple
Quadruple create_quadruple(SymbolTable *symtab, char* op, char* arg1, char* arg2, char* result) {
    Quadruple quad = {
        .op = strdup(op),
        .arg1 = arg1 ? strdup(arg1) : NULL,
        .arg2 = arg2 ? strdup(arg2) : NULL,
        .result = strdup(result)
    };
    
    return quad;
}

/* Update the add_quadruple function to handle prev pointer */
Quadruple add_quadruple(SymbolTable* symtab, Quadruple quad) {
    if (!symtab) {
        // Handle null symbol table
        Quadruple empty = {NULL, NULL, NULL, NULL};
        return empty;
    }

    QuadrupleNode* new_node = (QuadrupleNode*)malloc(sizeof(QuadrupleNode));
    if (!new_node) {
        // Handle allocation failure
        Quadruple empty = {NULL, NULL, NULL, NULL};
        return empty;
    }

    // Deep copy the quadruple data
    new_node->quad.op = quad.op ? strdup(quad.op) : NULL;
    new_node->quad.arg1 = quad.arg1 ? strdup(quad.arg1) : NULL;
    new_node->quad.arg2 = quad.arg2 ? strdup(quad.arg2) : NULL;
    new_node->quad.result = quad.result ? strdup(quad.result) : NULL;
    new_node->next = NULL;
    new_node->prev = symtab->quad_tail;  // Set the prev pointer to current tail
    
    // Add to list
    if (symtab->quad_tail == NULL) {
        symtab->quad_head = symtab->quad_tail = new_node;
    } else {
        symtab->quad_tail->next = new_node;
        symtab->quad_tail = new_node;
    }
    
    symtab->quad_count++;
    return new_node->quad;
}

// Iterate over quadruples
void iterate_quadruples(SymbolTable* symtab, void (*callback)(Quadruple*, void*), void* user_data) {
    QuadrupleNode* current = symtab->quad_head;
    while (current != NULL) {
        callback(&current->quad, user_data);
        current = current->next;
    }
}

/*BackPatch function to fill in If-Else labels */
void BackPatchLabels(SymbolTable* symtab, char* else_label, char* next_label) {
    if (!symtab) return;
    
    QuadrupleNode* current = symtab->quad_tail;
    while (current != NULL) {
        if (strcmp(current->quad.arg1, "UNFILLED ELSE")==0){
            free(current->quad.arg1);
            current->quad.arg1 = strdup(else_label);
            break;
        }
        current = current->prev;
    }

    QuadrupleNode* current2 = symtab->quad_tail;
    while (current2 != NULL) {
        if (strcmp(current2->quad.arg1, "UNFILLED NEXT")==0){
            free(current2->quad.arg1);
            current2->quad.arg1 = strdup(next_label);
            break;
        }
        current2 = current2->prev;
    }
}

char* create_temp(SymbolTable *symtab) {
    char* buf = (char*)malloc(100); // Allocate memory for the temporary name
    if (!buf) {
        return NULL;
    }
    
    snprintf(buf, 100, "t%d", symtab->current_temp_index++);
    
    Symbol* temp = (Symbol*)malloc(sizeof(Symbol));
    if (!temp) {
        free(buf);
        return NULL;
    }
    
    temp->kind = SYMBOL_TEMPORARY;
    temp->name = strdup(buf); // Make a copy since we'll return buf
    symtab_insert(symtab, temp);
    
    return buf;
}

/* ----------------------------------------------------------------------------------------------------------------------------- */

/* ------------------------------------------------- Printing Helper Functions ------------------------------------------------- */

/* Print the contents of the symbol table to an output file */
void symtab_fprint(SymbolTable *symtab, FILE* output) {
    // Print Symbol Table
    fprintf(output, "\nSymbol Table Contents:\n");
    fprintf(output, "--------------------------------------------------\n");
    fprintf(output, "%-20s %-10s %-10s %-10s\n", "Name", "Kind", "Type", "Scope");
    fprintf(output, "--------------------------------------------------\n");
    char* kind[6] = {"VARIABLE", "FUNCTION", "PARAMETER", "CONSTANT", "TYPE", "TEMPORARY"};
    char* type[10] = {"UNKNOWN", "VOID", "INT", "REAL", "CHAR", "IDENTIFIER", "BOOL", "ARRAY", "STRUCT"};

    for (int i = 0; i < symtab->size; i++) {
        if (symtab->table[i].is_occupied && !symtab->table[i].is_deleted) {
            fprintf(output, "%-22s %-10s %-10s %-10d\n",
                   symtab->table[i].name,
                   kind[symtab->table[i].kind],
                   type[symtab->table[i].type],
                   symtab->table[i].scope);
        }
    }
    fprintf(output, "--------------------------------------------------\n\n");

    // Print Quadruples
    fprintf(output, "Quadruples List:\n");
    fprintf(output, "--------------------------------------------------\n");
    fprintf(output, "%-5s %-10s %-15s %-15s %-15s\n", "No.", "Op", "Arg1", "Arg2", "Result");
    fprintf(output, "--------------------------------------------------\n");
    
    QuadrupleNode* current = symtab->quad_head;
    int quad_num = 1;
    
    while (current != NULL) {
        fprintf(output, "%-5d %-10s %-15s %-15s %-15s\n",
               quad_num++,
               current->quad.op ? current->quad.op : "-",
               current->quad.arg1 ? current->quad.arg1 : "-",
               current->quad.arg2 ? current->quad.arg2 : "-",
               current->quad.result ? current->quad.result : "-");
        current = current->next;
    }
    fprintf(output, "--------------------------------------------------\n");
    
}
/* Print the contents of the symbol table */
void symtab_print(SymbolTable *symtab) {
    // Print Symbol Table
    printf("\nSymbol Table Contents:\n");
    printf("--------------------------------------------------\n");
    printf("%-20s %-10s %-10s %-10s\n", "Name", "Kind", "Type", "Scope");
    printf("--------------------------------------------------\n");
    char* kind[6] = {"VARIABLE", "FUNCTION", "PARAMETER", "CONSTANT", "TYPE", "TEMPORARY"};
    char* type[10] = {"UNKNOWN", "VOID", "INT", "REAL", "CHAR", "IDENTIFIER", "BOOL", "ARRAY", "STRUCT"};

    for (int i = 0; i < symtab->size; i++) {
        if (symtab->table[i].is_occupied && !symtab->table[i].is_deleted) {
            printf("%-22s %-10s %-10s %-10d\n",
                   symtab->table[i].name,
                   kind[symtab->table[i].kind],
                   type[symtab->table[i].type],
                   symtab->table[i].scope);
        }
    }
    printf("--------------------------------------------------\n\n");

    // Print Quadruples
    printf("Quadruples List:\n");
    printf("--------------------------------------------------\n");
    printf("%-5s %-10s %-15s %-15s %-15s\n", "No.", "Op", "Arg1", "Arg2", "Result");
    printf("--------------------------------------------------\n");
    
    QuadrupleNode* current = symtab->quad_head;
    int quad_num = 1;
    
    while (current != NULL) {
        printf("%-5d %-10s %-15s %-15s %-15s\n",
               quad_num++,
               current->quad.op ? current->quad.op : "-",
               current->quad.arg1 ? current->quad.arg1 : "-",
               current->quad.arg2 ? current->quad.arg2 : "-",
               current->quad.result ? current->quad.result : "-");
        current = current->next;
    }
    printf("--------------------------------------------------\n");
    
}

// Example callback function to print quadruples
void print_quadruple(Quadruple* quad, void* user_data) {
    printf("(%s, %s, %s, %s)\n", 
           quad->op ? quad->op : "null",
           quad->arg1 ? quad->arg1 : "null",
           quad->arg2 ? quad->arg2 : "null",
           quad->result ? quad->result : "null");
}

void print_TAC(Quadruple* quad, void* user_data) {
    if (strcmp(quad->op, "GOTO")==0) {
        printf("\t %s %s\n", strdup(quad->op),strdup(quad->arg1));
    }
    else if (strcmp(quad->op, "LABEL")==0)
    {
        printf("%s:\n", strdup(quad->arg1));
    }
    else if (strcmp(quad->op, "IF")==0)
    {
        printf("\t %s %s GOTO %s \n", strdup(quad->op),strdup(quad->arg1), strdup(quad->result));
    }
    else if (strcmp(quad->op, ":=")==0)
    {
        printf("\t %s %s %s \n", strdup(quad->result),strdup(quad->op), strdup(quad->arg1));
    }
    else if (strcmp(quad->op, "RETURN")==0)
    {
        printf("\t %s %s\n", strdup(quad->op),strdup(quad->arg1));
    }
    else
    {
        printf("\t%s := %s %s %s\n", strdup(quad->result), strdup(quad->arg1), strdup(quad->op), strdup(quad->arg2));
    }
}

void fprint_TAC(Quadruple* quad, void* user_data) {
    if (strcmp(quad->op, "GOTO")==0) {
        fprintf(user_data,"\t %s %s\n", strdup(quad->op),strdup(quad->arg1));
    }
    else if (strcmp(quad->op, "LABEL")==0)
    {
        fprintf(user_data,"%s:\n", strdup(quad->arg1));
    }
    else if (strcmp(quad->op, "IF")==0)
    {
        fprintf(user_data,"\t %s %s GOTO %s \n", strdup(quad->op),strdup(quad->arg1), strdup(quad->result));
    }
    else if (strcmp(quad->op, ":=")==0)
    {
        fprintf(user_data,"\t %s %s %s \n", strdup(quad->result),strdup(quad->op), strdup(quad->arg1));
    }
    else if (strcmp(quad->op, "RETURN")==0)
    {
        fprintf(user_data,"\t %s %s\n", strdup(quad->op),strdup(quad->arg1));
    }
    else
    {
        fprintf(user_data,"\t%s := %s %s %s\n", strdup(quad->result), strdup(quad->arg1), strdup(quad->op), strdup(quad->arg2));
    }
}

/* ----------------------------------------------------------------------------------------------------------------------------- */
