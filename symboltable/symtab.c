#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define TABLE_SIZE 10000  // Prime number to help with distribution.
#define LOAD_FACTOR 0.7 // Load Factor, to know when to resize.


// Hash Function: return hash value for a string
unsigned int hash(const char *key, int table_size) {
    unsigned int hash = 0;
    for (int i = 0; key[i] != '\0'; i++){
        hash = (hash + key[i]) % table_size;
        }
    return hash;
}

// Create a hash table instance
HashTable* create_table(int size) {
    HashTable *table = malloc(sizeof(HashTable));
    table->symbols = calloc(size, sizeof(Symbol)); // Set all entries to 0 initially.
    table->size = size;
    table->count = 0;
    return table;
}

// resize the hash table when the load factor is reached
void resize(HashTable *table);

// Insert a pair (key, value) into hash table
int insert(HashTable *table, const char *key, void* value, SymbolType type, int lineno, int row) {
    if ((float)table->count / table->size >= LOAD_FACTOR){
        resize(table);
    }

    unsigned int index = hash(key, table->size);

    while (table->symbols[index].is_occupied == 1){
        index = (index+1) % table->size; // linear probing.
    }

    // inset new symbol
    table->symbols[index].key = strdup(key);
    table->symbols[index].value = value;
    table->symbols[index].type = type;
    table->symbols[index].is_occupied = 1;
    table->symbols[index].line_num = lineno;
    table->symbols[index].row = row;
    table->count++;


    return index;
}


// Search within a hash table.
Symbol search(HashTable *table, const char *key){
    unsigned int index = hash(key, table->size);

    while (table->symbols[index].is_occupied != 0){
        if (table->symbols[index].is_occupied == 1 && strcmp(table->symbols[index].key, key) == 0){
            return table->symbols[index];
        }
        index = (index+1) % table->size; // linear
    }
   
    // Return an "empty" symbol if not found
    Symbol empty_symbol = {NULL, 0, NULL, -1, -1, 0};  // Create an empty Symbol with no key and no value
    return empty_symbol;
}

// delete a key from a hash table.
void delete(HashTable *table, const char *key){
    unsigned int index = hash(key, table->size);

    while (table->symbols[index].is_occupied != 0){
        if (table->symbols[index].is_occupied == 1 && strcmp(table->symbols[index].key, key) == 0){
            table->symbols[index].is_occupied = -1; // mark as deleted.
            free(table->symbols[index].key);
            table->count--;
            return;
        }
        index = (index+1) % table->size; // linear
    }
}


// Resize the table when the size limit (load factor) is reached.
void resize(HashTable *table){
// create a new table with new size.
    int newSize = table->size *2;
    Symbol *oldSymbols = table->symbols;
    int oldSize = table->size;
    
    table->symbols = calloc(newSize, sizeof(Symbol));
    table->size = newSize;
    table->count = 0;

    //Rehashing all of the occupied symbols
    for (int i=0; i< oldSize; i++){
        if (oldSymbols[i].is_occupied == 1){
            insert(table, oldSymbols[i].key, oldSymbols[i].value, oldSymbols[i].type, oldSymbols[i].line_num, oldSymbols[i].row);
            free(oldSymbols[i].key); // free the old keys.
        }
    }
    free(oldSymbols);
}

// Free the hash table
void free_table(HashTable *table) {
    for (int i = 0; i < table->size; i++) {
        if (table->symbols[i].is_occupied == 1) {
            free(table->symbols[i].key);  // Free allocated keys
        }
    }
    free(table->symbols);  // Free entry array
    free(table);  // Free table structure
}

/* Example usage of Hash Table */

/*
int main(){
    HashTable *table = create_table(200);
    // Store the value by allocating memory for it
    int *value = malloc(sizeof(int));
    *value = 12;
    insert(table, "x", value);
    
    Symbol klid = search(table, "x");
    int s = *(int*)klid.value;
    printf("Retrieved value: %d\n", s);
    free_table(table);
    return 0;
}
*/
