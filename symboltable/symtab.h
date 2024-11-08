#ifndef SYMTAB_H
#define SYMTAB_H

// Define the size of the symbol table
#define TABLE_SIZE 5011  // Prime number to help with distribution

// Define the structure of a Symbol
typedef struct {
    char *key;      // Symbol name (e.g., variable name)
    int data;       // Symbol data (e.g., type, scope level, etc.)
    int is_occupied;  // 1 if the slot is occupied, 0 if it is empty
} Symbol;

// Declare the hash table as an array of pointers to Symbol entries
extern Symbol* hash_table[TABLE_SIZE];

// Function declarations for symbol table operations

// Primary hash function for symbol keys
unsigned int primary_hash(const char *key);

// Secondary hash function for double hashing
unsigned int secondary_hash(const char *key);

// Function to calculate the probing sequence using double hashing
unsigned int double_hash_probe(const char *key, int i);

// Insert a symbol into the symbol table
void insert(const char *key, int data);

// Search for a symbol by its key in the symbol table
int search(const char *key);

// Delete a symbol by its key from the symbol table
void delete(const char *key);

// Free all memory associated with the symbol table
void free_table();

#endif // SYMTAB_H
