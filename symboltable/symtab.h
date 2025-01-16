#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>

/* Types of symbols that can be stored in the symbol table */
typedef enum {
    SYMBOL_VARIABLE,    // Variables
    SYMBOL_FUNCTION,    // Functions
    SYMBOL_PARAMETER,   // Function parameters
    SYMBOL_CONSTANT,    // Constants
    SYMBOL_TYPE,        // User-defined types
    SYMBOL_TEMPORARY    // Temporary variables for code generation
} SymbolKind;

/* Data type of the symbol */
typedef enum {
    TYPE_UNKNOWN,
    TYPE_VOID,
    TYPE_INT,
    TYPE_REAL,
    TYPE_CHAR,
    TYPE_IDENTIFIER,
    TYPE_BOOL,
    TYPE_ARRAY,
    TYPE_STRUCT
} DataType;

/* Scope information for managing nested scopes */
typedef struct Scope {
    int scope_level;           // Nesting level of the scope
    int scope_id;             // Unique identifier for this scope
} Scope;

/* Symbol structure containing all necessary information */
typedef struct Symbol {
    char *name;               // Name/identifier of the symbol
    SymbolKind kind;         // Kind of symbol (variable, function, etc.)
    DataType type;           // Data type of the symbol
    Scope scope;             // Scope information
    
    // Memory layout information
    int offset;              // Memory offset for code generation
    int size;                // Size of the symbol in bytes
    
    // Additional attributes
    bool is_initialized;     // Whether the symbol has been initialized
    bool is_constant;        // Whether the symbol is a constant
    bool is_parameter;       // Whether the symbol is a parameter
    int param_position;      // Position of parameter in function call
    
    // Source location for error reporting
    int line_number;         // Line where symbol was declared
    int column_number;       // Column where symbol was declared
    
    // For linear probing collision resolution
    bool is_occupied;        // Whether this slot is occupied
    bool is_deleted;         // Marked for deletion (tombstone)
} Symbol;

/* Hash table structure */
typedef struct SymbolTable {
    Symbol *table;           // Array of symbol entries
    int size;               // Current size of the table
    int count;              // Number of occupied entries
    float load_factor;      // Maximum load factor before resizing
} SymbolTable;

/* Function declarations */
SymbolTable* symtab_create(int initial_size);
void symtab_destroy(SymbolTable *symtab);
bool symtab_insert(SymbolTable *symtab, const Symbol *symbol);
Symbol* symtab_lookup(SymbolTable *symtab, const char *name, int scope_id);
void symtab_delete(SymbolTable *symtab, const char *name, int scope_id);
void symtab_enter_scope(SymbolTable *symtab);
void symtab_exit_scope(SymbolTable *symtab);
void symtab_print(SymbolTable *symtab);

#endif // SYMTAB_H
