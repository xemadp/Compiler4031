# Compiler4031
K.N Toosi University of Technology Compiler 4031 Fall Project

# Lexer
The lexical analyzer generator `flex` is used to tokenize the input streams into tokens in the project specification format.
A symbol table is contained within the implementation of this lexer, to help with error handling in the future.

# Parser
The Parser is made using `bison` and has the symtab.c integrated in it to manage identifiers upon declarations.


# Symbol Table


# Intermediate Representation

Intermediate representation will be done in the parser phase of compiler, as a rule is being reduced, we can generate the quadruple for that IR there.
This ofcourse will need access to Symbol Table and good ST management.
Here is a diagram showcasing how the three address codes can be : 
```mermaid
graph TD
    A[Three Address Codes]
    B[Assignment Operations]
    C[Copy Assignments]
    D[Binary Assignments]
    E1["t = 12"]
    E2["t = a"]
    E3["..."]
    F1["t = 12 * 12"]
    F2["t = a + b"]
    F3["t = a + 12"]
    F4["t = 12 * b"]
    F5["..."]

    A --> B
    B --> C
    B --> D
    C --> E1
    C --> E2
    C --> E3
    D --> F1
    D --> F2
    D --> F3
    D --> F4
    D --> F5
```

Also we can utilize a system of Linked lists in order to show the relation between the quadruples, here is an example: 

input: 
```
a := 2 + c * 12
```

hence the quadruples are created in order :
```
Three Adress codes         Quadruples
t1 = c * 12        <->   (*, c, 12, t1) -> Binary Assignment
t2 = 12 + t1       <->   (+, 12, t1, t2) -> Binary Assignment
a  = t2            <->   (=, t2, -, a) -> Copy Assignment
```
Now Consider this linked list: 

```
(*, c, 12, t1) -> (+, 12, t1, t2) -> (=, t2, -, a) -> NULL
```

we can see that the three adress codes can be created seemlessly from this Linked List.
Within our implementation we will be identifying and creating the Quadruples, then placing them in SymbolTable.
The quadruple q can be later be adressed by q.result. doing this means we link them at each step.
In the end, we enumerate the linked lists and then print the corresponding listing of three adress codes.

```
t1 = c * 12 
t2 = 12 + t1     <-> a := 2 + c * 12
a  = t2     
```

## Test Cases
Some Programs written using the program specification, to use for Lexer.
they have been written to test the lexer against and can be found in the `lexer/TestCases` directory.

# TODO


- [ ] Implement Intermediate Code Generation into parser, translate.y
    - [ ] Create a Struct to represent quadruples in symtab.h
    - [ ] Create a way to connect quadruples together -> Linked Lists.
    - [ ] Everytime a reduction is performed, create and add the corresponding quadruple into Symbol Table.
    - [ ] In the end, create a way to move through all the quadruples in order and print them into an output.txt file
- [x] Write Syntax Analyzer Generator in yacc/bison.
- [x] Change and Integrate Symbol Table in parser.
- [x] Write Lexical Analyzer Generator in lex/flex.
- [x] Add Manual Lexer Output for the Lexer to be checked against.
- [x] Add Symbol Table Functionality using hashtable implementation.
- [x] Integrate Symbol Table into lexer.
- [x] Add proper pattern rules to flex program.
