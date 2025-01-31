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

/* Necessary information for the Symbol structure */
typedef struct Symbol {
    char *name;              // Name/identifier of the symbol
    SymbolKind kind;         // Kind of symbol (variable, function, etc.)
    DataType type;           // Data type of the symbol
    int scope;             // Scope information
    
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

/* struct to show Quadruple */
typedef struct Quadruple {
    char* op;
    char* arg1;
    char* arg2;
    char* result;
} Quadruple;

typedef struct QuadrupleNode {
    Quadruple quad;
    struct QuadrupleNode* next;
    struct QuadrupleNode* prev;
} QuadrupleNode;

/* Hash table structure */
typedef struct SymbolTable {
    Symbol *table;           // Array of symbol entries
    int size;                // Current size of the table
    int count;               // Number of occupied entries
    float load_factor;       // Maximum load factor before resizing
    int current_temp_index;  // to use for t_i when generating Intermediate Representation.
                            
    //Quadruple tracking
    QuadrupleNode* quad_head;    // Head of quadruple list
    QuadrupleNode* quad_tail;    // Tail for O(1) insertion
    int quad_count;              // Number of quadruples
                                 
} SymbolTable;

typedef struct LabelPair {
    char* label1;
    char* label2;
    char* label3;
} LabelPair;


/* Function declarations */
SymbolTable* symtab_create(int initial_size);
void symtab_destroy(SymbolTable *symtab);
bool symtab_insert(SymbolTable *symtab, const Symbol *symbol);
Symbol* symtab_lookup(SymbolTable *symtab, const char *name, int scope_id);
void symtab_delete(SymbolTable *symtab, const char *name, int scope_id);
void symtab_print(SymbolTable *symtab); // print symboltable
void symtab_fprint(SymbolTable *symtab, FILE* output); // print symbol table to output file

/* Function declarations related to Intermediate Representation Generation */

// generate result as newTemp or find in symboltable, pass to add_quadruple
Quadruple create_quadruple(SymbolTable *symtab, char* op, char* arg1, char* arg2, char* result);
char* create_temp(SymbolTable *symtab); // create a temporary var in symboltable
Quadruple add_quadruple(SymbolTable* symtab, Quadruple quad); // add quad to ST
void iterate_quadruples(SymbolTable* symtab, void (*callback)(Quadruple*, void*), void* user_data); // iterate
void print_quadruple(Quadruple* quad, void* user_data); // Helper function to print a quadruple in the SymbolTable LinkedList
void print_TAC(Quadruple* quad, void* user_data); // Helper function to print the three address code for a quad
void fprint_TAC(Quadruple* quad, void* user_data); // Helper function to print the three address code for a quad in an outputFILE
void BackPatchLabels(SymbolTable* symtab, char* else_label, char* next_label); // backpatch the else and next labels.

#endif // SYMTAB_H
