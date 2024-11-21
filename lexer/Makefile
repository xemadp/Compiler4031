# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lfl

# Source files
LEXER_SRC = lex.l
SYMTAB_SRC = ../symboltable/symtab.c

# Generated files
LEXER_GEN = lex.yy.c

# Output binary
TARGET = lexer

# Rules
all: $(TARGET)

$(TARGET): $(LEXER_GEN) $(SYMTAB_SRC)
	$(CC) $(CFLAGS) -o $@ $(LEXER_GEN) $(SYMTAB_SRC) $(LDFLAGS)

$(LEXER_GEN): $(LEXER_SRC)
	flex $<

clean:
	rm -f $(LEXER_GEN) $(TARGET)
