# Modular C Compiler & Virtual Machine

A lightweight, multi-file compiler and stack-based Virtual Machine built in C. Translates a custom high-level procedural language into **Three-Address Code (Quadruples)** and executes it on a custom-built VM.

---

## Project Structure

```
HW3/
├── tiny_compiler.h    # Shared header: token enum, IR struct, global declarations
├── lexer.c            # Lexical analyzer — source code → token stream
├── parser.c           # Recursive-descent parser — grammar rules → IR emission
├── codegen.c          # IR code generator — emits quadruples into program_ir[]
├── vm.c               # Virtual Machine — stack-based interpreter for the IR
└── main.c             # Entry point — file I/O, pipeline orchestration
```

---

## Compilation Pipeline

```
Source File (*.txt)
       │
       ▼
  ┌─────────┐
  │  LEXER  │  advance_token() — strips whitespace/comments, produces Token stream
  └────┬────┘
       │
       ▼
  ┌─────────┐
  │ PARSER  │  compile_program() — recursive-descent parsing with operator precedence
  └────┬────┘       emits IR instructions via emit_ir()
       │
       ▼
  ┌─────────┐
  │ CODEGEN │  program_ir[0..999] — array of IR_Instr quadruples
  └────┬────┘       format: (opcode, arg1, arg2, result)
       │
       ▼
  ┌─────────┐
  │   VM    │  execute_vm() — fetch-decode-execute loop with call stack
  └─────────┘       prints global variables at exit
```

---

## Language Grammar

```
program       → { func_def | statement }

func_def      → "func" ID "(" [ ID { "," ID } ] ")" "{" { statement } "}"

statement     → if_stmt | while_stmt | assign_stmt | return_stmt

if_stmt       → "if" "(" expression ")" "{" { statement } "}"
while_stmt    → "while" "(" expression ")" "{" { statement } "}"

assign_stmt   → ID "=" expression ";"
return_stmt   → "return" expression ";"

expression    → comp_expr { ( "==" | "<" | ">" ) comp_expr }
comp_expr     → term { ( "+" | "-" ) term }
term          → factor { ( "*" | "/" ) factor }

factor        → NUM | ID [ "(" [ expression { "," expression } ] ")" ] | "(" expression ")"
```

### Operator Precedence (lowest → highest)

| Level | Operators        | Associativity |
|-------|------------------|---------------|
| 1     | `==` `<` `>`     | left          |
| 2     | `+` `-`          | left          |
| 3     | `*` `/`          | left          |

### Keywords

| Keyword  | Purpose             |
|----------|---------------------|
| `func`   | Define a function   |
| `return` | Return from function|
| `if`     | Conditional branch  |
| `while`  | Loop construct      |

---

## Token Types (`TokenKind` enum in `tiny_compiler.h:12`)

| Enum Value    | Source          | Description         |
|---------------|-----------------|---------------------|
| `TOK_FUNC`    | `func`          | function keyword    |
| `TOK_RET`     | `return`        | return keyword      |
| `TOK_IF`      | `if`            | if keyword          |
| `TOK_WHILE`   | `while`         | while keyword       |
| `TOK_ID`      | `[a-zA-Z_]+`   | identifier          |
| `TOK_NUM`     | `[0-9]+`       | integer literal     |
| `TOK_LPAREN`  | `(`             | left parenthesis    |
| `TOK_RPAREN`  | `)`             | right parenthesis   |
| `TOK_LBRACE`  | `{`             | left brace          |
| `TOK_RBRACE`  | `}`             | right brace         |
| `TOK_COMMA`   | `,`             | comma separator     |
| `TOK_SEMI`    | `;`             | semicolon           |
| `TOK_ASSIGN`  | `=`             | assignment          |
| `TOK_ADD`     | `+`             | addition            |
| `TOK_SUB`     | `-`             | subtraction         |
| `TOK_MUL`     | `*`             | multiplication      |
| `TOK_DIV`     | `/`             | division            |
| `TOK_EQ`      | `==`            | equality comparison |
| `TOK_LT`      | `<`             | less than           |
| `TOK_GT`      | `>`             | greater than        |
| `TOK_EOF`     | (end of input)  | end of file         |

---

## IR Opcode Reference

Each IR instruction is a 4-field `IR_Instr` struct (`tiny_compiler.h:33`):

```
(opcode, arg1, arg2, result)
```

### Data Movement

| Opcode     | arg1    | arg2 | result       | Description                              |
|------------|---------|------|--------------|------------------------------------------|
| `LOAD_IMM` | literal | -    | temp/var     | Load integer literal into variable       |
| `STORE`    | src_var | -    | dest_var     | Copy value from src_var to dest_var      |

### Arithmetic

| Opcode | arg1    | arg2    | result | Description                           |
|--------|---------|---------|--------|---------------------------------------|
| `ADD`  | var/num | var/num | temp   | result = arg1 + arg2                  |
| `SUB`  | var/num | var/num | temp   | result = arg1 - arg2                  |
| `MUL`  | var/num | var/num | temp   | result = arg1 * arg2                  |
| `DIV`  | var/num | var/num | temp   | result = arg1 / arg2                  |

### Comparison

| Opcode  | arg1    | arg2    | result | Description                          |
|---------|---------|---------|--------|--------------------------------------|
| `IS_EQ` | var/num | var/num | temp   | result = (arg1 == arg2) ? 1 : 0      |
| `IS_LT` | var/num | var/num | temp   | result = (arg1 <  arg2) ? 1 : 0      |
| `IS_GT` | var/num | var/num | temp   | result = (arg1 >  arg2) ? 1 : 0      |

### Control Flow

| Opcode      | arg1      | arg2 | result | Description                           |
|-------------|-----------|------|--------|---------------------------------------|
| `JMP`       | -         | -    | target | Unconditional jump to PC = target     |
| `JMP_FALSE` | condition | -    | target | Jump to target if condition == 0      |

### Functions

| Opcode       | arg1      | arg2     | result     | Description                             |
|--------------|-----------|----------|------------|-----------------------------------------|
| `DEF_FUNC`   | func_name | -        | -          | Marks start of function body            |
| `END_FUNC`   | func_name | -        | -          | Marks end of function body              |
| `PUSH_ARG`   | var/num   | -        | -          | Push argument onto arg_buffer           |
| `CALL`       | func_name | arg_count| result_var | Call function; store return in result   |
| `BIND_PARAM` | param_name| -        | -          | Bind a pushed argument to parameter     |
| `RETURN`     | var/num   | -        | -          | Return value from current frame         |

---

## Build & Run

### Compile

```bash
gcc main.c lexer.c parser.c codegen.c vm.c -o my_compiler
```

### Run

```bash
./my_compiler program.txt
```

### Example Program

```c
// factorial via loop
counter = 5;
accumulator = 1;

while (counter > 0) {
    accumulator = accumulator * counter;
    counter = counter - 1;
}

print(accumulator);
```

### Expected Output

```
Intermediate Representation (IR):
--------------------------------------------
000: LOAD_IMM   5          -          t1
001: STORE      t1         -          counter
002: LOAD_IMM   1          -          t2
003: STORE      t2         -          accumulator
004: LOAD_IMM   counter    -          t3
005: LOAD_IMM   0          -          t4
006: IS_GT      t3         t4         t5
007: JMP_FALSE  t5         -          ?
008: LOAD_IMM   accumulator-          t6
009: LOAD_IMM   counter    -          t7
010: MUL        t6         t7         t8
011: STORE      t8         -          accumulator
...
=== VM Execution Started ===
=== VM Execution Finished ===

Global Variables:
>> counter = 0
>> accumulator = 120
```

---

## Internals

### Backpatching Strategy

For `if` and `while` statements, the jump destination is unknown when first emitted. The parser:

1. Emits `JMP_FALSE` with result `"?"` as a placeholder at index `N`
2. Parses the body block
3. After the closing `}`, patches `program_ir[N].result` with the correct PC

For `while` loops, an additional unconditional `JMP` back to the loop header is emitted and patched as well.

Key code path: `parser.c:103-108` (if) and `parser.c:117-125` (while).

### Virtual Machine Call Stack

The VM uses an explicit `ExecutionFrame` stack (`vm.c:3-11`) to isolate function scopes:

1. **CALL**: Pushes a new frame (`frame_ptr++`), saves return PC and target variable, copies arguments from arg buffer
2. **BIND_PARAM**: Binds passed arguments to local variables in the current frame
3. **RETURN**: Stores return value into the caller's target variable, pops the frame (`frame_ptr--`), jumps back to `return_pc`
4. **DEF_FUNC/END_FUNC**: VM skips over function bodies during global execution (they run only when called)

Global variables live in `call_stack[0]` (frame 0) and are printed at program termination.

### Temp Variable Convention

Temporaries are named `t1`, `t2`, ... `tN` via `generate_temp()` in `parser.c:5`. The VM filters them out when displaying global variables (any variable whose name starts with `t` is hidden).

### Comment Handling

Lexer supports both `//` single-line and `/* */` multi-line comments, skipped during tokenization (`lexer.c:14-25`).
