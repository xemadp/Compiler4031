#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* Constants for hash table management */
#define INITIAL_SIZE 101        // Prime number for better distribution
#define MAX_LOAD_FACTOR 0.7     // Resize when table is 70% full
#define GROWTH_FACTOR 2         // Double size when resizing

/* Private function declarations */
static unsigned int hash_function(const char *str, int scope_id, int table_size);
static bool symtab_resize(SymbolTable *symtab);

/* Hash function using Jenkins hash for better distribution */
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
    hash += (hash << 10);
    hash ^= (hash >> 6);
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return hash % table_size;
}

/* Create new symbol table */
SymbolTable* symtab_create(int initial_size) {
    SymbolTable *symtab = malloc(sizeof(SymbolTable));
    if (!symtab) return NULL;
    
    symtab->table = calloc(initial_size, sizeof(Symbol));
    if (!symtab->table) {
        free(symtab);
        return NULL;
    }
    
    symtab->size = initial_size;
    symtab->count = 0;
    symtab->load_factor = MAX_LOAD_FACTOR;
    
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
                                            symtab->table[i].scope.scope_id, 
                                            new_size);
            
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
    
    unsigned int index = hash_function(symbol->name, symbol->scope.scope_id, symtab->size);
    
    // Linear probe for empty slot or tombstone
    while (symtab->table[index].is_occupied && !symtab->table[index].is_deleted) {
        // Check for duplicate in same scope
        if (strcmp(symtab->table[index].name, symbol->name) == 0 &&
            symtab->table[index].scope.scope_id == symbol->scope.scope_id) {
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
            symtab->table[index].scope.scope_id == scope_id) {
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
            symtab->table[index].scope.scope_id == scope_id) {
            
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
    
    free(symtab->table);
    free(symtab);
}

/* Print the contents of the symbol table (useful for debugging) */
void symtab_print(SymbolTable *symtab) {
    printf("\nSymbol Table Contents:\n");
    printf("--------------------------------------------------\n");
    printf("%-20s %-10s %-10s %-10s\n", "Name", "Kind", "Type", "Scope");
    printf("--------------------------------------------------\n");
    
    for (int i = 0; i < symtab->size; i++) {
        if (symtab->table[i].is_occupied && !symtab->table[i].is_deleted) {
            printf("%-22s %-10d %-10d %-10d\n",
                   symtab->table[i].name,
                   symtab->table[i].kind,
                   symtab->table[i].type,
                   symtab->table[i].scope.scope_id);
        }
    }
    printf("--------------------------------------------------\n");
}
