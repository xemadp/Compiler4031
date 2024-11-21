# Symbol Table
A symbol table, implemented in C to be used within flex/bison and during the three phases of the compiler project.


## Hash table structure

In this implementation I will try and implement a symbol table based on a hashing table with the following structure:

Using a hash table, since the tokens are limited and this way storage becomes less of a issue and it's more efficient. This way we can maintain average constant time complexity for insertions, lookups and deletions.

### Hash Function
A simple and quick hash function can really help with overhead in the table, meaning it's ideal for spreading the symbols accros the table.

### Probing Method
Linear probing is used to add entries to the symbol table in a linear fashion that matches the project phase 1 specification.

### Deletion Remarks
Standard Deletion methods are used to delete an entry from the hash table struct.
