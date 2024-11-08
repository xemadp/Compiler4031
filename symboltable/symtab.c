#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 5011  // Prime number to help with distribution

typedef struct {
    char *key;      // Symbol name 
    int data;       // Symbol data (e.g., type or scope level)
    int is_occupied;  // 1 if the slot is occupied, 0 if it is empty
} Symbol;

// Initialize the hash table as an array of pointers to Symbol entries
Symbol* hash_table[TABLE_SIZE] = {NULL};  

// Primary hash function
unsigned int primary_hash(const char *key) {
    unsigned int hash_val = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash_val = 31 * hash_val + key[i];
    }
    return hash_val % TABLE_SIZE;
}

// Secondary hash function for double hashing
unsigned int secondary_hash(const char *key) {
    unsigned int hash_val = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash_val = 17 * hash_val + key[i];
    }
    return (hash_val % (TABLE_SIZE - 1)) + 1; // Secondary hash must be positive integer
}

// Function to calculate the probing sequence
unsigned int double_hash_probe(const char *key, int i) {
    return (primary_hash(key) + i * secondary_hash(key)) % TABLE_SIZE;
}

// Insert a symbol into the hash table
void insert(const char *key, int data) {
    unsigned int index;
    int i = 0;

    // Probe until we find an empty slot
    do {
        index = double_hash_probe(key, i);
        if (hash_table[index] == NULL || !hash_table[index]->is_occupied) {
            // Allocate memory for the new symbol and insert it
            if (hash_table[index] == NULL) {
                hash_table[index] = malloc(sizeof(Symbol));
            }
            hash_table[index]->key = strdup(key);
            hash_table[index]->data = data;
            hash_table[index]->is_occupied = 1;
            printf("Inserted symbol '%s' at index %d\n", key, index);
            return;
        } else if (strcmp(hash_table[index]->key, key) == 0) {
            // Update data if the key already exists
            hash_table[index]->data = data;
            printf("Updated symbol '%s' at index %d\n", key, index);
            return;
        }
        i++;
    } while (i < TABLE_SIZE);

    printf("Error: Hash table is full\n");
}

// Search for a symbol by key
int search(const char *key) {
    unsigned int index;
    int i = 0;

    while (i < TABLE_SIZE) {
        index = double_hash_probe(key, i);
        if (hash_table[index] == NULL) {
            return -1;  // Symbol not found
        } else if (hash_table[index]->is_occupied && strcmp(hash_table[index]->key, key) == 0) {
            return hash_table[index]->data;
        }
        i++;
    }
    return -1;  // Symbol not found after full probe
}

// Delete a symbol by key
void delete(const char *key) {
    unsigned int index;
    int i = 0;

    while (i < TABLE_SIZE) {
        index = double_hash_probe(key, i);
        if (hash_table[index] == NULL) {
            printf("Symbol '%s' not found\n", key);
            return;
        } else if (hash_table[index]->is_occupied && strcmp(hash_table[index]->key, key) == 0) {
            // Mark the entry as deleted
            hash_table[index]->is_occupied = 0;
            printf("Deleted symbol '%s' at index %d\n", key, index);
            return;
        }
        i++;
    }
    printf("Symbol '%s' not found\n", key);
}

// Free the memory associated with the hash table
void free_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hash_table[i] != NULL) {
            free(hash_table[i]->key);
            free(hash_table[i]);
        }
    }
}

// Main function to test the implementation
int main() {
    insert("x", 1);
    insert("y", 2);
    insert("z", 3);

    // Search for symbols
    int result = search("x");
    if (result != -1) {
        printf("Found symbol 'x' with data: %d\n", result);
    } else {
        printf("Symbol 'x' not found.\n");
    }

    result = search("y");
    if (result != -1) {
        printf("Found symbol 'y' with data: %d\n", result);
    } else {
        printf("Symbol 'y' not found.\n");
    }

    result = search("a");  // Search for a symbol that does not exist
    if (result != -1) {
        printf("Found symbol 'a' with data: %d\n", result);
    } else {
        printf("Symbol 'a' not found.\n");
    }

    // Delete a symbol
    delete("y");
    result = search("y");
    if (result != -1) {
        printf("Found symbol 'y' with data: %d\n", result);
    } else {
        printf("Symbol 'y' not found.\n");
    }

    // Free the hash table
    free_table();
    return 0;
}
