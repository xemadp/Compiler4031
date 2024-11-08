#ifndef SYMTAB_H
#define SYMTAB_H

// Defining symbol structure
typedef struct Symbol {
    char *key;
    int data;
    int is_occupied;
} Symbol;

// Symbol table functions list
void symtab_insert(const char *key, int data);
int symtab_lookup(const char *key);
void symtab_delete(const char *key);
void free_symtab();

#endif
