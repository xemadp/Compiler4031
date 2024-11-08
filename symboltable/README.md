# Symbol Table
A symbol table, implemented in C to be used within flex/bison and during the three phases of the compiler project.


## Hash table structure

In this implementation I will try and implement a symbol table based on a hashing table with the following structure:

Using a hash table with open addressing, since the tokens are limited and this way storage becomes less of a issue and it's more efficient. This way we can maintain average constant time complexity for insertions, lookups and deletions.

### Hash Function
A simple and quick hash function can really help with overhead in the table, meaning it's ideal for spreading the symbols accros the table.

### Probing Method
Double Hashing is the most optimized method for probing in this scenario since it gives the most even distribution of entries.
It also performs better for higher load factors. Meaning way less rehashes are required.
It utilizes an additional hash function to make sure all entries have a positive integer value.

### Deletion Remarks
Deleting from the symbol table needs special handling to avoid breaking the probing sequence.
We will use a Tombstone Marker to ensure proper probing after deletion.
