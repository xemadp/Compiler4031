# Compiler and tools
CC = gcc
YACC = bison
LEX = flex
# Compiler and linker flags
CFLAGS = -Wall -g
LDFLAGS = -lfl
# Directories
LEXER_DIR = ../lexer
SYMTAB_DIR = ../symboltable
# Source files
LEXER_SRC = $(LEXER_DIR)/lex.yy.c
PARSER_SRC = translate.tab.c
SYMTAB_SRC = $(SYMTAB_DIR)/symtab.c
# Object files
OBJECTS = $(LEXER_SRC:.c=.o) $(PARSER_SRC:.c=.o) $(SYMTAB_SRC:.c=.o)
# Final executable name
TARGET = parser
# Default target
all: $(TARGET)
# Rule to build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)
	@echo "Build complete! You can now run './$(TARGET)'"
# Generate parser source from .y file
translate.tab.c translate.tab.h: translate.y
	$(YACC) -d translate.y
	@echo "Generated parser source files"
# Generate lexer source from .l file
$(LEXER_SRC): $(LEXER_DIR)/lex.l translate.tab.h
	$(LEX) -o $(LEXER_SRC) $(LEXER_DIR)/lex.l
	@echo "Generated lexer source file"
# Generic rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
# Clean generated files
clean:
	rm -f $(TARGET) $(OBJECTS) translate.tab.c translate.tab.h $(LEXER_SRC)
	@echo "Cleaned all generated files"
# Prevent make from removing intermediate files
.PRECIOUS: translate.tab.c translate.tab.h $(LEXER_SRC)
# Add header dependencies
$(LEXER_DIR)/lex.yy.o: $(LEXER_SRC) translate.tab.h
translate.tab.o: translate.tab.c
$(SYMTAB_DIR)/symtab.o: $(SYMTAB_DIR)/symtab.c $(SYMTAB_DIR)/symtab.h
# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build the parser (default)"
	@echo "  clean      - Remove generated files"
	@echo "  help       - Show this help message"
.PHONY: all clean help
