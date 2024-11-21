# Language Specification
[BNF](https://www.github.com/xemadp/compiler4031/blob/master/specification/specification.bnf) (Backus-Naur Form), [EBNF](https://www.github.com/xemadp/compiler4031/blob/master/specification/specification.ebnf) for the language provided in the [project declaration](https://www.github.com/xemadp/Compiler4031/blob/master/phase1.pdf).

Here are diagrams for each production in the grammar.

**start:**

![start](diagram/start.png)

```
start    ::= 'program' id ';' decList funcList block
```

**decList:**

![decList](diagram/decList.png)

```
decList  ::= decs decList*
```

referenced by:

* decList
* funcDec
* parameters
* start

**decs:**

![decs](diagram/decs.png)

```
decs     ::= type varList ';'
           | ''
```

referenced by:

* decList

**type:**

![type](diagram/type.png)

```
type     ::= 'integer'
           | 'real'
           | 'boolean'
```

referenced by:

* decs
* funcDec

**varList:**

![varList](diagram/varList.png)

```
varList  ::= id varList*
```

referenced by:

* decs
* varList

**funcList:**

![funcList](diagram/funcList.png)

```
funcList ::= funcDec
           | ''
```

referenced by:

* start

**funcDec:**

![funcDec](diagram/funcDec.png)

```
funcDec  ::= 'function' id parameters ':' type decList block
```

referenced by:

* funcList

**parameters:**

![parameters](diagram/parameters.png)

```
parameters
         ::= '(' decList ')'
```

referenced by:

* funcDec

**block:**

![block](diagram/block.png)

```
block    ::= 'begin' stmtList 'end'
```

referenced by:

* funcDec
* start
* stmt

**stmtList:**

![stmtList](diagram/stmtList.png)

```
stmtList ::= stmt stmtList*
```

referenced by:

* block
* stmtList

**stmt:**

![stmt](diagram/stmt.png)

```
stmt     ::= ( id ':=' | 'return' ) expr ';'
           | ( 'if' expr 'then' ( stmt 'else' )? | ( 'while' | 'for' id ':=' expr 'to' ) expr 'do' ) stmt
           | block
```

referenced by:

* stmt
* stmtList

**expr:**

![expr](diagram/expr.png)

```
expr     ::= expr ( 'and' | 'or' | '*' | '/' | '+' | '-' | relop ) expr
           | '(' expr ')'
           | integerNumber
           | realNumber
           | 'true'
           | 'false'
           | id ( '(' actualparamlist ')' )?
```

referenced by:

* actualparamlist
* expr
* stmt

**actualparamlist:**

![actualparamlist](diagram/actualparamlist.png)

```
actualparamlist
         ::= expr actualparamlist*
           | id
           | ''
```

referenced by:

* actualparamlist
* expr

**relop:**

![relop](diagram/relop.png)

```
relop    ::= '<'
           | '<='
           | '='
           | '<>'
           | '>='
           | '>'
```

referenced by:

* expr

**id:**

![id](diagram/id.png)

```
id       ::= letter ( letter | digit )+
```

referenced by:

* actualparamlist
* expr
* funcDec
* start
* stmt
* varList

**letter:**

![letter](diagram/letter.png)

```
letter   ::= 'A'
           | 'B'
           | 'C'
           | 'D'
           | 'E'
           | 'F'
           | 'G'
           | 'H'
           | 'I'
           | 'J'
           | 'K'
           | 'L'
           | 'M'
           | 'N'
           | 'O'
           | 'P'
           | 'Q'
           | 'R'
           | 'S'
           | 'T'
           | 'U'
           | 'V'
           | 'W'
           | 'X'
           | 'Y'
           | 'Z'
           | 'a'
           | 'b'
           | 'c'
           | 'd'
           | 'e'
           | 'f'
           | 'g'
           | 'h'
           | 'i'
           | 'j'
           | 'k'
           | 'l'
           | 'm'
           | 'n'
           | 'o'
           | 'p'
           | 'q'
           | 'r'
           | 's'
           | 't'
           | 'u'
           | 'v'
           | 'w'
           | 'x'
           | 'y'
           | 'z'
```

referenced by:

* id

**nonZeroDigit:**

![nonZeroDigit](diagram/nonZeroDigit.png)

```
nonZeroDigit
         ::= '1'
           | '2'
           | '3'
           | '4'
           | '5'
           | '6'
           | '7'
           | '8'
           | '9'
```

referenced by:

* digit
* noLeadingInt

**digit:**

![digit](diagram/digit.png)

```
digit    ::= '0'
           | nonZeroDigit
```

referenced by:

* digitSequence
* id

**integerNum:**

![integerNum](diagram/integerNum.png)

```
integerNum
         ::= sign noLeadingInt
```

referenced by:

* decimalPlace
* realNumber

**sign:**

![sign](diagram/sign.png)

```
sign     ::= '+'
           | '-'
           | ''
```

referenced by:

* integerNum

**noLeadingInt:**

![noLeadingInt](diagram/noLeadingInt.png)

```
noLeadingInt
         ::= nonZeroDigit digitSequence
```

referenced by:

* integerNum

**digitSequence:**

![digitSequence](diagram/digitSequence.png)

```
digitSequence
         ::= digit digitSequence*
```

referenced by:

* decimalPlace
* digitSequence
* noLeadingInt

**realNumber:**

![realNumber](diagram/realNumber.png)

```
realNumber
         ::= integerNum '.' decimalPlace
```

referenced by:

* expr

# Lexeme & Token List
[list](https://www.github.com/xemadp/compiler4031/blob/master/specification/token_list.txt) of Lexmes and their corresponding Tokens provided in the [project declaration](https://www.github.com/xemadp/Compiler4031/blob/master/phase1.pdf).

| **LEXEMES**       | **TOKEN_NAME**     |
|--------------------|--------------------|
| `program`         | `PROGRAM_KW`       |
| `function`        | `FUNCTION_KW`      |
| `begin`           | `BEGIN_KW`         |
| `end`             | `END_KW`           |
| `while`           | `WHILE_KW`         |
| `do`              | `DO_KW`            |
| `to`              | `TO_KW`            |
| `if`              | `IF_KW`            |
| `then`            | `THEN_KW`          |
| `else`            | `ELSE_KW`          |
| `integer`         | `INTEGER_KW`       |
| `real`            | `REAL_KW`          |
| `boolean`         | `BOOLEAN_KW`       |
| `return`          | `RETURN_KW`        |
| `and`             | `AND_KW`           |
| `or`              | `OR_KW`            |
| `false`           | `FALSE_KW`         |
| `id`              | `IDENTIFIER`       |
| `integerNumber`   | `INTEGER_NUMBER`   |
| `realNumber`      | `REAL_NUMBER`      |
| `:=`              | `ASSIGN_OP`        |
| `*`               | `MUL_OP`           |
| `/`               | `DIV_OP`           |
| `+`               | `ADD_OP`           |
| `-`               | `SUB_OP`           |
| `<`               | `LT_OP`            |
| `<=`              | `LE_OP`            |
| `<>`              | `NE_OP`            |
| `=`               | `EQ_OP`            |
| `>=`              | `GE_OP`            |
| `>`               | `GT_OP`            |
| `:`               | `COLON`            |
| `;`               | `SEMICOLON`        |
| `,`               | `COMMA`            |
| `(`               | `RIGHT_PA`         |
| `)`               | `LEFT_PA`          |

