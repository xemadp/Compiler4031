#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum {
    SYMBOL_TYPE_KEYWORD,
    SYMBOL_TYPE_IDENTIFIER,
    SYMBOL_TYPE_CONSTANT
} SymbolType;

typedef struct Symbol {
    char *key;
    SymbolType type;
    void *value;
    int line_num;
    int column;
    int is_occupied;
    int row;
} Symbol;

typedef struct HashTable {
    Symbol *symbols;
    int size;
    int count;
} HashTable;

HashTable* create_table(int size);
unsigned int hash(const char *key, int table_size);
int insert(HashTable *table, const char *key, void* value, SymbolType type, int lineno, int row);
Symbol search(HashTable *table, const char *key);
void delete(HashTable *table, const char *key);
void resize(HashTable *table);
void free_table(HashTable *table);

#endif // SYMTAB_H
