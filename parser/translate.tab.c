/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "translate.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../symboltable/symtab.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
void yyerror(const char* s);

/* Global Variables */
SymbolTable* symboltable;  
FILE* output;
FILE* TACoutput;
#define MAX_SCOPES 100 // Maximum number of nested scopes
DataType current_type;    // Tracks current declaration type

int label_count = 0;
char label_buffer[20];

/* Function to generate new label */
char* new_label() {
    snprintf(label_buffer, sizeof(label_buffer), "L%d", label_count++);
    return strdup(label_buffer);
}

/* Symbol Table Management Functions */
void init_symbol_table() {
    symboltable = symtab_create(1000);  // Create with reasonable initial size
    if (!symboltable) {
        fprintf(stderr, "Failed to create symbol table\n");
        exit(1);
    }
}


int scope_stack[MAX_SCOPES]; // Stack to hold scope IDs
int top = -1;                // Index of the top of the stack
int next_scope_id = 1;       // Unique ID generator

// Push a scope ID onto the stack
void push_scope(int scope_id) {
    if (top >= MAX_SCOPES - 1) {
        fprintf(stderr, "Scope stack overflow\n");
        exit(EXIT_FAILURE);
    }
    scope_stack[++top] = scope_id;
}

// Pop the top scope ID off the stack
void pop_scope() {
    if (top < 0) {
        fprintf(stderr, "Scope stack underflow\n");
        exit(EXIT_FAILURE);
    }
    top--;
}

// Get the current scope ID (top of the stack)
int current_scope() {
    if (top < 0) {
        fprintf(stderr, "No active scope\n");
        exit(EXIT_FAILURE);
    }
    return scope_stack[top];
}

// Enter a new scope
void enter_scope() {
    int new_scope_id = next_scope_id++; // Generate a unique ID
    push_scope(new_scope_id);          // Push the new scope onto the stack
    //printf("Entered scope %d\n", new_scope_id);
}

// Exit the current scope
void exit_scope() {
    //printf("Exiting scope %d\n", current_scope());
    pop_scope(); // Pop the current scope from the stack
}

// function declaration, basically only saves the identifier that is the name of a function.
// Program name is considered a function name.
void declare_function(const char* name) {
    Symbol* existing = symtab_lookup(symboltable, name, current_scope());
    if (existing && existing->kind==SYMBOL_FUNCTION) {
        fprintf(stderr, "Error at line %d: Redeclaration of function '%s' in current scope\n",
                yylineno, name);
        return;
    }

    Symbol new_symbol = {
        .name = strdup(name),
        .type = current_type,
        .kind = SYMBOL_FUNCTION,
        .scope = current_scope(),
        .line_number = yylineno,
        .is_initialized = false
    };
    
    if (!symtab_insert(symboltable, &new_symbol)) {
        fprintf(stderr, "Error at line %d: Failed to insert function name '%s' into symbol table\n",
                yylineno, name);
        free(new_symbol.name);
    }
}

void declare_variable(const char* name) {
    Symbol* existing = symtab_lookup(symboltable, name, current_scope());
    if (existing) {
        fprintf(stderr, "Error at line %d: Redeclaration of variable '%s' in current scope\n",
                yylineno, name);
        return;
    }
    Symbol new_symbol = {
        .name = strdup(name),
        .type = current_type,
        .kind = SYMBOL_VARIABLE,
        .scope = current_scope(),
        .line_number = yylineno,
        .is_initialized = false
    };
    
    if (!symtab_insert(symboltable, &new_symbol)) {
        fprintf(stderr, "Error at line %d: Failed to insert variable '%s' into symbol table\n",
                yylineno, name);
        free(new_symbol.name);
    }
}

// Print Current reduction
void print_reduction(int rule_num, const char* rule_text) {
    fprintf(output, "%d\t%s\n", rule_num, rule_text);
}


#line 209 "translate.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "translate.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_PROGRAM_KW = 3,                 /* PROGRAM_KW  */
  YYSYMBOL_FUNCTION_KW = 4,                /* FUNCTION_KW  */
  YYSYMBOL_BEGIN_KW = 5,                   /* BEGIN_KW  */
  YYSYMBOL_END_KW = 6,                     /* END_KW  */
  YYSYMBOL_IF_KW = 7,                      /* IF_KW  */
  YYSYMBOL_WHILE_KW = 8,                   /* WHILE_KW  */
  YYSYMBOL_DO_KW = 9,                      /* DO_KW  */
  YYSYMBOL_FOR_KW = 10,                    /* FOR_KW  */
  YYSYMBOL_TO_KW = 11,                     /* TO_KW  */
  YYSYMBOL_THEN_KW = 12,                   /* THEN_KW  */
  YYSYMBOL_ELSE_KW = 13,                   /* ELSE_KW  */
  YYSYMBOL_INTEGER_KW = 14,                /* INTEGER_KW  */
  YYSYMBOL_REAL_KW = 15,                   /* REAL_KW  */
  YYSYMBOL_BOOLEAN_KW = 16,                /* BOOLEAN_KW  */
  YYSYMBOL_RETURN_KW = 17,                 /* RETURN_KW  */
  YYSYMBOL_AND_KW = 18,                    /* AND_KW  */
  YYSYMBOL_OR_KW = 19,                     /* OR_KW  */
  YYSYMBOL_ASSIGN_OP = 20,                 /* ASSIGN_OP  */
  YYSYMBOL_MUL_OP = 21,                    /* MUL_OP  */
  YYSYMBOL_DIV_OP = 22,                    /* DIV_OP  */
  YYSYMBOL_ADD_OP = 23,                    /* ADD_OP  */
  YYSYMBOL_SUB_OP = 24,                    /* SUB_OP  */
  YYSYMBOL_LT_OP = 25,                     /* LT_OP  */
  YYSYMBOL_LE_OP = 26,                     /* LE_OP  */
  YYSYMBOL_NE_OP = 27,                     /* NE_OP  */
  YYSYMBOL_EQ_OP = 28,                     /* EQ_OP  */
  YYSYMBOL_GE_OP = 29,                     /* GE_OP  */
  YYSYMBOL_GT_OP = 30,                     /* GT_OP  */
  YYSYMBOL_COLON = 31,                     /* COLON  */
  YYSYMBOL_SEMICOLON = 32,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 33,                     /* COMMA  */
  YYSYMBOL_LEFT_PA = 34,                   /* LEFT_PA  */
  YYSYMBOL_RIGHT_PA = 35,                  /* RIGHT_PA  */
  YYSYMBOL_INTEGER_NUMBER = 36,            /* INTEGER_NUMBER  */
  YYSYMBOL_REAL_NUMBER = 37,               /* REAL_NUMBER  */
  YYSYMBOL_TRUE_KW = 38,                   /* TRUE_KW  */
  YYSYMBOL_FALSE_KW = 39,                  /* FALSE_KW  */
  YYSYMBOL_IDENTIFIER = 40,                /* IDENTIFIER  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_start = 42,                     /* start  */
  YYSYMBOL_43_1 = 43,                      /* $@1  */
  YYSYMBOL_decList = 44,                   /* decList  */
  YYSYMBOL_decs = 45,                      /* decs  */
  YYSYMBOL_type = 46,                      /* type  */
  YYSYMBOL_varList = 47,                   /* varList  */
  YYSYMBOL_funcList = 48,                  /* funcList  */
  YYSYMBOL_funcDec = 49,                   /* funcDec  */
  YYSYMBOL_parameters = 50,                /* parameters  */
  YYSYMBOL_block = 51,                     /* block  */
  YYSYMBOL_stmtList = 52,                  /* stmtList  */
  YYSYMBOL_stmt = 53,                      /* stmt  */
  YYSYMBOL_54_2 = 54,                      /* @2  */
  YYSYMBOL_55_3 = 55,                      /* @3  */
  YYSYMBOL_56_4 = 56,                      /* @4  */
  YYSYMBOL_if_body = 57,                   /* if_body  */
  YYSYMBOL_58_5 = 58,                      /* $@5  */
  YYSYMBOL_expr = 59,                      /* expr  */
  YYSYMBOL_actualParamList = 60,           /* actualParamList  */
  YYSYMBOL_relop = 61                      /* relop  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   167

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  54
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  102

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   295


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   177,   177,   176,   190,   194,   201,   206,   212,   218,
     223,   231,   236,   245,   250,   256,   264,   271,   278,   282,
     289,   298,   297,   346,   345,   377,   376,   436,   440,   447,
     452,   451,   469,   479,   489,   500,   510,   521,   531,   541,
     546,   551,   556,   561,   566,   570,   578,   582,   587,   593,
     597,   601,   605,   609,   613
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "PROGRAM_KW",
  "FUNCTION_KW", "BEGIN_KW", "END_KW", "IF_KW", "WHILE_KW", "DO_KW",
  "FOR_KW", "TO_KW", "THEN_KW", "ELSE_KW", "INTEGER_KW", "REAL_KW",
  "BOOLEAN_KW", "RETURN_KW", "AND_KW", "OR_KW", "ASSIGN_OP", "MUL_OP",
  "DIV_OP", "ADD_OP", "SUB_OP", "LT_OP", "LE_OP", "NE_OP", "EQ_OP",
  "GE_OP", "GT_OP", "COLON", "SEMICOLON", "COMMA", "LEFT_PA", "RIGHT_PA",
  "INTEGER_NUMBER", "REAL_NUMBER", "TRUE_KW", "FALSE_KW", "IDENTIFIER",
  "$accept", "start", "$@1", "decList", "decs", "type", "varList",
  "funcList", "funcDec", "parameters", "block", "stmtList", "stmt", "@2",
  "@3", "@4", "if_body", "$@5", "expr", "actualParamList", "relop", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-32)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      12,   -31,    21,   -32,   -32,    -5,     4,   -32,   -32,   -32,
     -32,     4,    -1,    31,   -32,   -32,     5,     1,     6,   -32,
     -32,   -32,     3,    19,    11,    11,    17,    11,    38,   -32,
       0,   -32,   -32,     4,    28,    11,   -32,   -32,   -32,   -32,
      26,   114,   114,    41,    84,    11,   -32,   -32,    27,     4,
      69,    11,    11,    11,    11,    11,    11,    11,   -32,   -32,
     -32,   -32,   -32,   -32,    53,    11,    57,    11,   -32,    99,
     -32,     4,   -32,   114,    -9,   137,   127,   -32,   -32,    34,
      34,     6,   114,     6,    56,   -32,    65,    11,   -32,    59,
     -32,   -32,    11,   -32,   114,   -32,   114,     6,    67,   -32,
       6,   -32
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     1,     0,     7,     8,     9,    10,
      14,     4,     0,     0,     5,    11,     0,     0,     0,    13,
       3,     6,     0,     0,     0,     0,     0,     0,     0,    28,
       0,    18,    12,     7,     0,     0,    40,    41,    42,    43,
      45,    21,    23,     0,     0,     0,    17,    19,     0,     0,
       0,    48,     0,     0,     0,     0,     0,     0,    49,    50,
      52,    51,    53,    54,     0,     0,     0,     0,    27,     0,
      16,     7,    39,    46,     0,    32,    33,    34,    35,    36,
      37,     0,    38,     0,     0,    20,     0,     0,    44,    29,
      22,    24,     0,    15,    47,    30,    25,     0,     0,    31,
       0,    26
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -32,   -32,   -32,    -8,   -32,    40,   -32,   -32,   -32,   -32,
     -13,   -32,   -29,   -32,   -32,   -32,   -32,   -32,   -23,   -32,
     -32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     5,    10,    11,    12,    16,    13,    19,    34,
      29,    30,    31,    64,    66,    98,    90,    97,    41,    74,
      65
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      20,    47,    42,    14,    44,    18,    46,    24,    25,     3,
      26,    18,    50,    24,    25,     1,    26,    27,     7,     8,
       9,     4,    69,    27,    87,    48,    88,     6,    73,    75,
      76,    77,    78,    79,    80,    17,    18,    21,    22,    15,
      28,    23,    82,    32,    84,    35,    28,    36,    37,    38,
      39,    40,    89,    33,    91,    54,    55,    43,    45,    49,
      51,    67,    70,    86,    94,    81,    83,    92,    99,    96,
      18,   101,    95,    93,    52,    53,   100,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    52,    53,    71,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
       0,     0,    52,    53,    72,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,     0,    68,    52,    53,     0,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
       0,    85,    52,    53,     0,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    52,     0,     0,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63
};

static const yytype_int8 yycheck[] =
{
      13,    30,    25,    11,    27,     5,     6,     7,     8,    40,
      10,     5,    35,     7,     8,     3,    10,    17,    14,    15,
      16,     0,    45,    17,    33,    33,    35,    32,    51,    52,
      53,    54,    55,    56,    57,     4,     5,    32,    33,    40,
      40,    40,    65,    40,    67,    34,    40,    36,    37,    38,
      39,    40,    81,    34,    83,    21,    22,    40,    20,    31,
      34,    20,    35,    71,    87,    12,     9,    11,    97,    92,
       5,   100,    13,    86,    18,    19,     9,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    18,    19,    49,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    -1,    18,    19,    35,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    32,    18,    19,    -1,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    32,    18,    19,    -1,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    18,    -1,    -1,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    42,    40,     0,    43,    32,    14,    15,    16,
      44,    45,    46,    48,    44,    40,    47,     4,     5,    49,
      51,    32,    33,    40,     7,     8,    10,    17,    40,    51,
      52,    53,    40,    34,    50,    34,    36,    37,    38,    39,
      40,    59,    59,    40,    59,    20,     6,    53,    44,    31,
      59,    34,    18,    19,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    54,    61,    55,    20,    32,    59,
      35,    46,    35,    59,    60,    59,    59,    59,    59,    59,
      59,    12,    59,     9,    59,    32,    44,    33,    35,    53,
      57,    53,    11,    51,    59,    13,    59,    58,    56,    53,
       9,    53
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    43,    42,    44,    44,    45,    45,    46,    46,
      46,    47,    47,    48,    48,    49,    50,    51,    52,    52,
      53,    54,    53,    55,    53,    56,    53,    53,    53,    57,
      58,    57,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    60,    60,    60,    61,
      61,    61,    61,    61,    61
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     7,     1,     2,     3,     0,     1,     1,
       1,     1,     3,     2,     0,     7,     3,     3,     1,     2,
       4,     0,     5,     0,     5,     0,     9,     3,     1,     1,
       0,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     4,     1,     1,     3,     0,     1,
       1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 177 "translate.y"
    {
    enter_scope();
    declare_function((yyvsp[0].id_name));
    }
#line 1339 "translate.tab.c"
    break;

  case 3: /* start: PROGRAM_KW IDENTIFIER $@1 SEMICOLON decList funcList block  */
#line 183 "translate.y"
    {
        print_reduction(yyn-2, "start -> PROGRAM_KW IDENTIFIER SEMICOLON decList funcList block");
        exit_scope();
    }
#line 1348 "translate.tab.c"
    break;

  case 4: /* decList: decs  */
#line 191 "translate.y"
    {
        print_reduction(yyn-2, "decList -> decs");
    }
#line 1356 "translate.tab.c"
    break;

  case 5: /* decList: decs decList  */
#line 195 "translate.y"
    {
        print_reduction(yyn-2, "decList -> decs decList");
    }
#line 1364 "translate.tab.c"
    break;

  case 6: /* decs: type varList SEMICOLON  */
#line 202 "translate.y"
    {
        print_reduction(yyn-2, "decs -> type varList SEMICOLON");
    }
#line 1372 "translate.tab.c"
    break;

  case 7: /* decs: %empty  */
#line 206 "translate.y"
    {
        print_reduction(yyn-2, "decs -> epsilon");
    }
#line 1380 "translate.tab.c"
    break;

  case 8: /* type: INTEGER_KW  */
#line 213 "translate.y"
    {
        current_type = TYPE_INT;
        print_reduction(yyn-2, "type -> INTEGER_KW");
        
    }
#line 1390 "translate.tab.c"
    break;

  case 9: /* type: REAL_KW  */
#line 219 "translate.y"
    {
        current_type = TYPE_REAL;
        print_reduction(yyn-2, "type -> REAL_KW");
    }
#line 1399 "translate.tab.c"
    break;

  case 10: /* type: BOOLEAN_KW  */
#line 224 "translate.y"
    {
        current_type = TYPE_BOOL;
        print_reduction(yyn-2, "type -> BOOLEAN_KW");
    }
#line 1408 "translate.tab.c"
    break;

  case 11: /* varList: IDENTIFIER  */
#line 232 "translate.y"
    {
        declare_variable((yyvsp[0].id_name));
        print_reduction(yyn-2, "varList -> IDENTIFIER");
    }
#line 1417 "translate.tab.c"
    break;

  case 12: /* varList: varList COMMA IDENTIFIER  */
#line 237 "translate.y"
    {
        declare_variable((yyvsp[0].id_name));
        print_reduction(yyn-2, "varList -> varList COMMA IDENTIFIER");
    }
#line 1426 "translate.tab.c"
    break;

  case 13: /* funcList: funcList funcDec  */
#line 246 "translate.y"
    {
        print_reduction(yyn-2, "funcList -> funcList funcDec");
    }
#line 1434 "translate.tab.c"
    break;

  case 14: /* funcList: %empty  */
#line 250 "translate.y"
    {
        print_reduction(yyn-2, "funcList -> epsilon");
    }
#line 1442 "translate.tab.c"
    break;

  case 15: /* funcDec: FUNCTION_KW IDENTIFIER parameters COLON type decList block  */
#line 257 "translate.y"
    {
        declare_function((yyvsp[-5].id_name));
        print_reduction(yyn-2, "funcDec -> FUNCTION_KW IDENTIFIER parameters COLON type decList block");
    }
#line 1451 "translate.tab.c"
    break;

  case 16: /* parameters: LEFT_PA decList RIGHT_PA  */
#line 265 "translate.y"
    {
        print_reduction(yyn-2, "parameters -> LEFT_PA decList RIGHT_PA");
    }
#line 1459 "translate.tab.c"
    break;

  case 17: /* block: BEGIN_KW stmtList END_KW  */
#line 272 "translate.y"
    {
        print_reduction(yyn-2, "block -> BEGIN_KW stmtList END_KW");
    }
#line 1467 "translate.tab.c"
    break;

  case 18: /* stmtList: stmt  */
#line 279 "translate.y"
    {
        print_reduction(yyn-2, "stmtList -> stmt");
    }
#line 1475 "translate.tab.c"
    break;

  case 19: /* stmtList: stmtList stmt  */
#line 283 "translate.y"
    {
        print_reduction(yyn-2, "stmtList -> stmtList stmt");
    }
#line 1483 "translate.tab.c"
    break;

  case 20: /* stmt: IDENTIFIER ASSIGN_OP expr SEMICOLON  */
#line 290 "translate.y"
    {
        Quadruple quad = create_quadruple(symboltable,":=",(yyvsp[-1].id_name),"", (yyvsp[-3].id_name));
        add_quadruple(symboltable,quad);

        //printf("%s := %s\n", $1, $3);
        print_reduction(yyn-2, "stmt -> IDENTIFIER ASSIGN_OP expr SEMICOLON");
    }
#line 1495 "translate.tab.c"
    break;

  case 21: /* @2: %empty  */
#line 298 "translate.y"
    {
        char* TRUE_label = new_label();   // if cond is true
        char* NEXT_label = new_label();  // code after if stmt
        char* ELSE_label = new_label(); // if cond is not true hence else is true
        
        // Create Quadruples
        Quadruple ifquad = create_quadruple(symboltable,"IF",(yyvsp[0].id_name),"", TRUE_label);
        //printf("if (%s) goto %s\n", $2, TRUE_label);
        add_quadruple(symboltable, ifquad);

        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", NEXT_label, "", "");
        //printf("goto %s\n", NEXT_label);
        add_quadruple(symboltable, gotoquad);

        Quadruple labelquad = create_quadruple(symboltable, "LABEL", TRUE_label, "", "");
        //printf("%s : \n", TRUE_label);
        add_quadruple(symboltable, labelquad);
        
        (yyval.label_pair).label1 = NEXT_label;
        (yyval.label_pair).label2 = ELSE_label;
        (yyval.label_pair).label3 = TRUE_label;
        
    }
#line 1523 "translate.tab.c"
    break;

  case 22: /* stmt: IF_KW expr @2 THEN_KW if_body  */
#line 322 "translate.y"
      {
      if (strcmp((yyvsp[0].id_name),"hasElse")==0){
          printf("hasElse Check Passed!!!!\n");
          Quadruple labelquad = create_quadruple(symboltable, "LABEL",(yyvsp[-2].label_pair).label2, "", "");
          add_quadruple(symboltable, labelquad);
          //printf("%s : \n", NEXT_label);

          // BACKPATCH THE IF-ELSE LABELS!
          // Using the  backpatch function from symtab.c here.
         
          BackPatchLabels(symboltable, (yyvsp[-2].label_pair).label2, (yyvsp[-2].label_pair).label1);
      }
      else
      {
          printf("hasElse Check ---NOT--- Passed!!!!\n");
         Quadruple labelquad = create_quadruple(symboltable, "LABEL",(yyvsp[-2].label_pair).label1, "", "");
         //printf("%s : \n", NEXT_label);
         add_quadruple(symboltable, labelquad);
         //printf("%s : \n", $<label_pair>3.label1);
        
      }
      print_reduction(yyn-2, "stmt -> IF_KW expr THEN_KW stmt ELSE_KW stmt");
    }
#line 1551 "translate.tab.c"
    break;

  case 23: /* @3: %empty  */
#line 346 "translate.y"
    {
        char* WHILE_label = new_label(); // before loop starts
        char* TRUE_label = new_label();   // if loop cond is true
        char* FALSE_label = new_label();  // if loop cond is false
        (yyval.label_pair).label1 = WHILE_label;
        (yyval.label_pair).label2 = FALSE_label;

        Quadruple labelquad = create_quadruple(symboltable, "LABEL", WHILE_label, "", "");
        add_quadruple(symboltable, labelquad);
        //printf("%s : \n", WHILE_label);
        Quadruple ifquad = create_quadruple(symboltable, "IF", (yyvsp[0].id_name), "", TRUE_label);
        add_quadruple(symboltable, ifquad);
        //printf("if (%s) goto %s\n", $2, TRUE_label);
        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", FALSE_label, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", FALSE_label);
        Quadruple tlabelquad = create_quadruple(symboltable, "LABEL", TRUE_label, "", "");
        add_quadruple(symboltable, tlabelquad);
        //printf("%s : \n", TRUE_label);
    }
#line 1576 "translate.tab.c"
    break;

  case 24: /* stmt: WHILE_KW expr @3 DO_KW stmt  */
#line 367 "translate.y"
    {   
        Quadruple gotoquad = create_quadruple(symboltable, "GOTO",(yyvsp[-2].label_pair).label1, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", $<label_pair>3.label1);
        Quadruple flabelquad = create_quadruple(symboltable, "LABEL", (yyvsp[-2].label_pair).label2, "", "");
        add_quadruple(symboltable, flabelquad);
        //printf("%s : \n", $<label_pair>3.label2);
        print_reduction(yyn-2, "stmt -> WHILE_KW expr DO_KW stmt");
    }
#line 1590 "translate.tab.c"
    break;

  case 25: /* @4: %empty  */
#line 377 "translate.y"
    {
        enter_scope();
        declare_variable((yyvsp[-4].id_name));
        
        Quadruple quad = create_quadruple(symboltable,":=",(yyvsp[-2].id_name),"", (yyvsp[-4].id_name));
        add_quadruple(symboltable, quad);
        //printf("%s := %s\n", $2, $4);

        char* temp = create_temp(symboltable);

        // Add some quad here first for later
        Quadruple forCondQuad = create_quadruple(symboltable,"<=",(yyvsp[-4].id_name),(yyvsp[0].id_name), temp);
        add_quadruple(symboltable, forCondQuad);
        //printf("%s := %s <= %s\n", temp, $2, $6);
        
        char* CONDITION_label = new_label(); // Condition of For loop
        char* BODY_label = new_label(); // BODY of For loop
        char* NEXT_label = new_label(); // Code after For loop
        
        Quadruple clabelquad = create_quadruple(symboltable, "LABEL", CONDITION_label, "", "");
        add_quadruple(symboltable, clabelquad);
        //printf("%s : \n", CONDITION_label);

        Quadruple ifquad = create_quadruple(symboltable, "IF", temp, "", BODY_label);
        add_quadruple(symboltable, ifquad);
        //printf("if (%s) goto %s\n", temp, BODY_label);

        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", NEXT_label, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", NEXT_label);

        Quadruple blabelquad = create_quadruple(symboltable, "LABEL", BODY_label, "", "");
        add_quadruple(symboltable, blabelquad);
        //printf("%s : \n", BODY_label);

        (yyval.label_pair).label1 = CONDITION_label;
        (yyval.label_pair).label2 = strdup((yyvsp[-4].id_name));
        (yyval.label_pair).label3 = NEXT_label;
       

    }
#line 1636 "translate.tab.c"
    break;

  case 26: /* stmt: FOR_KW IDENTIFIER ASSIGN_OP expr TO_KW expr @4 DO_KW stmt  */
#line 419 "translate.y"
    {
        // for x:= 1 to n do ... -> x := x + 1 in end of for loop before going back to check the for cond.
        Quadruple forIncrementQuad = create_quadruple(symboltable,"+", (yyvsp[-2].label_pair).label2, "1", (yyvsp[-2].label_pair).label2);
        add_quadruple(symboltable, forIncrementQuad);
        //printf("%s := %s + 1 \n", $<label_pair>7.label2, $<label_pair>7.label2);

        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", (yyvsp[-2].label_pair).label1, "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto %s\n", $<label_pair>7.label1);

        Quadruple nlabelquad = create_quadruple(symboltable, "LABEL",(yyvsp[-2].label_pair).label3, "", "");
        add_quadruple(symboltable, nlabelquad);
        //printf("%s : \n", $<label_pair>7.label3);

        exit_scope();
        print_reduction(yyn-2, "stmt -> FOR IDENTIFIER ASSIGN_OP expr TO_KW expr DO_KW stmt");
    }
#line 1658 "translate.tab.c"
    break;

  case 27: /* stmt: RETURN_KW expr SEMICOLON  */
#line 437 "translate.y"
    {
        print_reduction(yyn-2, "stmt -> RETURN_KW expr SEMICOLON");
    }
#line 1666 "translate.tab.c"
    break;

  case 28: /* stmt: block  */
#line 441 "translate.y"
    {
        print_reduction(yyn-2, "stmt -> block");
    }
#line 1674 "translate.tab.c"
    break;

  case 29: /* if_body: stmt  */
#line 448 "translate.y"
    {
    // Do nothing.
    }
#line 1682 "translate.tab.c"
    break;

  case 30: /* $@5: %empty  */
#line 452 "translate.y"
    {
        printf("---------SEEEN ELSE IN INPUT------------\n");
        Quadruple gotoquad = create_quadruple(symboltable, "GOTO", "UNFILLED ELSE", "", "");
        add_quadruple(symboltable, gotoquad);
        //printf("goto  ---- <- NEXT LABEL FOR IF-ELSE STMT\n");

        Quadruple elabelquad = create_quadruple(symboltable, "LABEL","UNFILLED NEXT", "", "");
        add_quadruple(symboltable, elabelquad);
        //printf("LABEL ---- <- ELSE LABEL FOR IF-ELSE STMT \n");
    }
#line 1697 "translate.tab.c"
    break;

  case 31: /* if_body: stmt ELSE_KW $@5 stmt  */
#line 463 "translate.y"
    {
    (yyval.id_name) = "hasElse";
    // Do nothing.
    }
#line 1706 "translate.tab.c"
    break;

  case 32: /* expr: expr AND_KW expr  */
#line 470 "translate.y"
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"&&",(yyvsp[-2].id_name),(yyvsp[0].id_name), temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s && %s\n", $$, $1, $3);
            
        (yyval.id_name) = temp;
        print_reduction(yyn-2, "expr -> expr AND_KW expr");
    }
#line 1720 "translate.tab.c"
    break;

  case 33: /* expr: expr OR_KW expr  */
#line 480 "translate.y"
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"||",(yyvsp[-2].id_name),(yyvsp[0].id_name), temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s || %s\n", $$, $1, $3);
            
        (yyval.id_name) = temp;
        print_reduction(yyn-2, "expr -> expr OR_KW expr");
    }
#line 1734 "translate.tab.c"
    break;

  case 34: /* expr: expr MUL_OP expr  */
#line 490 "translate.y"
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"*",(yyvsp[-2].id_name),(yyvsp[0].id_name), temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s * %s\n", $$, $1, $3);
            
        (yyval.id_name) = temp;
        //printf("expr_1 -> expr_2 * expr_3 was reduced. where expr_1 is %s\n", temp);
        print_reduction(yyn-2, "expr -> expr MUL_OP expr");
    }
#line 1749 "translate.tab.c"
    break;

  case 35: /* expr: expr DIV_OP expr  */
#line 501 "translate.y"
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"/",(yyvsp[-2].id_name), (yyvsp[0].id_name), temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s / %s\n", $$, $1, $3);
            
        (yyval.id_name) = temp;
        print_reduction(yyn-2, "expr -> expr DIV_OP expr");
    }
#line 1763 "translate.tab.c"
    break;

  case 36: /* expr: expr ADD_OP expr  */
#line 511 "translate.y"
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"+",(yyvsp[-2].id_name),(yyvsp[0].id_name), temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s + %s\n", $$, $1, $3);
            
        (yyval.id_name) = temp;
        //printf("expr_1 -> expr_2 + expr_3 was reduced. where expr_1 is %s\n", temp);
        print_reduction(yyn-2, "expr -> expr ADD_OP expr");
    }
#line 1778 "translate.tab.c"
    break;

  case 37: /* expr: expr SUB_OP expr  */
#line 522 "translate.y"
    {
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,"-",(yyvsp[-2].id_name),(yyvsp[0].id_name), temp);
        add_quadruple(symboltable, quad);
        //printf("%s := %s - %s\n", $$, $1, $3);
            
        (yyval.id_name) = temp;
        print_reduction(yyn-2, "expr -> expr SUB_OP expr");
    }
#line 1792 "translate.tab.c"
    break;

  case 38: /* expr: expr relop expr  */
#line 532 "translate.y"
    {   
        char* temp = create_temp(symboltable);
        Quadruple quad = create_quadruple(symboltable,(yyvsp[-1].id_name),(yyvsp[-2].id_name),(yyvsp[0].id_name), temp);
        add_quadruple(symboltable, quad);
            
        (yyval.id_name) = temp;
        //printf("%s := %s %s %s\n", $$, $1, $2, $3);
        print_reduction(yyn-2, "expr -> expr relop expr");
    }
#line 1806 "translate.tab.c"
    break;

  case 39: /* expr: LEFT_PA expr RIGHT_PA  */
#line 542 "translate.y"
    {
        print_reduction(yyn-2, "expr -> LEFT_PA expr RIGHT_PA");
        (yyval.id_name) = (yyvsp[-1].id_name);
    }
#line 1815 "translate.tab.c"
    break;

  case 40: /* expr: INTEGER_NUMBER  */
#line 547 "translate.y"
    { 
        print_reduction(yyn-2, "expr -> INTEGER_NUMBER");
        (yyval.id_name) = (yyvsp[0].id_name);
    }
#line 1824 "translate.tab.c"
    break;

  case 41: /* expr: REAL_NUMBER  */
#line 552 "translate.y"
    {
        print_reduction(yyn-2, "expr -> REAL_NUMBER");
        (yyval.id_name) = (yyvsp[0].id_name);
    }
#line 1833 "translate.tab.c"
    break;

  case 42: /* expr: TRUE_KW  */
#line 557 "translate.y"
    {
        print_reduction(yyn-2, "expr -> TRUE_KW");
        (yyval.id_name) = (yyvsp[0].id_name);
    }
#line 1842 "translate.tab.c"
    break;

  case 43: /* expr: FALSE_KW  */
#line 562 "translate.y"
    {
        print_reduction(yyn-2, "expr -> FALSE_KW");
        (yyval.id_name) = (yyvsp[0].id_name);
    }
#line 1851 "translate.tab.c"
    break;

  case 44: /* expr: IDENTIFIER LEFT_PA actualParamList RIGHT_PA  */
#line 567 "translate.y"
    {
        print_reduction(yyn-2, "expr -> IDENTIFIER LEFT_PA actualParamList RIGHT_PA");
    }
#line 1859 "translate.tab.c"
    break;

  case 45: /* expr: IDENTIFIER  */
#line 571 "translate.y"
    {
        print_reduction(yyn-2, "expr -> IDENTIFIER");
        (yyval.id_name) = (yyvsp[0].id_name);
    }
#line 1868 "translate.tab.c"
    break;

  case 46: /* actualParamList: expr  */
#line 579 "translate.y"
    {
        print_reduction(yyn-2, "actualParamList -> expr");
    }
#line 1876 "translate.tab.c"
    break;

  case 47: /* actualParamList: actualParamList COMMA expr  */
#line 583 "translate.y"
    {
        print_reduction(yyn-2, "actualParamList -> actualParamList COMMA expr");
    }
#line 1884 "translate.tab.c"
    break;

  case 48: /* actualParamList: %empty  */
#line 587 "translate.y"
    {
        print_reduction(yyn-2, "actualParamList -> epsilon");
    }
#line 1892 "translate.tab.c"
    break;

  case 49: /* relop: LT_OP  */
#line 594 "translate.y"
    {
        print_reduction(yyn-2, "relop -> LT_OP");
    }
#line 1900 "translate.tab.c"
    break;

  case 50: /* relop: LE_OP  */
#line 598 "translate.y"
    {
        print_reduction(yyn-2, "relop -> LE_OP");
    }
#line 1908 "translate.tab.c"
    break;

  case 51: /* relop: EQ_OP  */
#line 602 "translate.y"
    {
        print_reduction(yyn-2, "relop -> EQ_OP");
    }
#line 1916 "translate.tab.c"
    break;

  case 52: /* relop: NE_OP  */
#line 606 "translate.y"
    {
        print_reduction(yyn-2, "relop -> NE_OP");
    }
#line 1924 "translate.tab.c"
    break;

  case 53: /* relop: GE_OP  */
#line 610 "translate.y"
    {
        print_reduction(yyn-2, "relop -> GE_OP");
    }
#line 1932 "translate.tab.c"
    break;

  case 54: /* relop: GT_OP  */
#line 614 "translate.y"
    {
        print_reduction(yyn-2, "relop -> GT_OP");
    }
#line 1940 "translate.tab.c"
    break;


#line 1944 "translate.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 619 "translate.y"


void yyerror(const char* s) {
    fprintf(stderr, "Error at line %d: %s near token '%s'\n", 
            yylineno, s, yytext);
}

int main(void) {
    // Initialize symbol table
    init_symbol_table();
    
    // Open input and output files
    yyin = fopen("input.txt", "r");
    output = fopen("parser-output.txt", "w");
    
    if (!yyin) {
        fprintf(stderr, "Error: Cannot open input file 'input.txt'\n");
        symtab_destroy(symboltable);
        return 1;
    }
    
    if (!output) {
        fprintf(stderr, "Error: Cannot create output file 'output.txt'\n");
        fclose(yyin);
        symtab_destroy(symboltable);
        return 1;
    }
    
    // Write NAME AND STUDENT NUMBER 
    fprintf(output, "Emad Pourhassani -> 40116623\n");
    fprintf(output, "Afarin Akhoundi  -> 40115283\n");
    fprintf(output, "----------------------------\n");
    
    // Parse input
    int result = yyparse();

    TACoutput = fopen("output.txt", "w");
    fprintf(TACoutput, "Emad Pourhassani := 40116623\n");
    fprintf(TACoutput, "Afarin Akhoundi  := 40115283\n");
    fprintf(TACoutput, "----------------------------\n");
    
    // Print final symbol table and quadruples to output.txt
    // generate TAC as well.
    symtab_fprint(symboltable, TACoutput);

    fprintf(TACoutput, "\nThree Address Codes:\n----------------------------\n");
    iterate_quadruples(symboltable, fprint_TAC, TACoutput);
    
    // Cleanup
    symtab_destroy(symboltable);
    fclose(yyin);
    fclose(output);
    
    return result;
}
