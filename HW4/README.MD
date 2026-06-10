# SP-Book: System Programming -- From Source to Hardware

> A book about system programming that IS a program. Written as a multi-file C project with an interactive CLI reader, structured chapters, companion demo programs, and export to text / HTML.

---

## Project Files

```
HW4/
├── spbook.h           # Header: Chapter/Section data structures, function declarations
├── book_content.c     # The book: 10 chapters, 40 sections of system programming content
├── reader.c           # Interactive CLI reader with navigation, text wrapping, export
├── main.c             # Entry point: interactive mode or CLI flags for direct access
├── demo_memory.c      # Demo: memory layout of a C program (Chapter 2)
├── demo_stack.c       # Demo: call stack frames (Chapters 2, 7)
├── demo_vm.c          # Demo: standalone stack VM (Chapter 7)
├── demo_lexer.c       # Demo: standalone lexer (Chapter 3)
└── README.md          # This file
```

---

## Quick Start

### Compile the book reader

```bash
gcc main.c book_content.c reader.c -o spbook
```

### Launch interactive mode

```bash
./spbook
```

### Direct chapter access

```bash
./spbook 3          # Read Chapter 3 (Lexical Analysis)
./spbook 4.2        # Read Section 4.2 (Operator Precedence)
./spbook --toc      # Print Table of Contents
./spbook --text     # Export full book to spbook.txt
./spbook --html     # Export full book to spbook.html
```

### Compile and run the demo programs

```bash
gcc demo_memory.c -o demo_memory && ./demo_memory
gcc demo_stack.c  -o demo_stack  && ./demo_stack
gcc demo_vm.c     -o demo_vm     && ./demo_vm
gcc demo_lexer.c  -o demo_lexer  && ./demo_lexer
```

---

## Interactive Reader Commands

| Command        | Action                                    |
|----------------|-------------------------------------------|
| `1` ... `10`   | Read a full chapter                       |
| `3.2` ... `9.4`| Read a specific section within a chapter  |
| `t`            | Show Table of Contents                    |
| `e`            | Export book to `spbook.txt`               |
| `h`            | Export book to `spbook.html`              |
| `q`            | Quit                                      |

---

## Table of Contents

| Ch | Title |
|----|-------|
| 1  | What Is System Programming? |
| 2  | The C Language for Systems |
| 3  | Lexical Analysis |
| 4  | Recursive-Descent Parsing |
| 5  | Intermediate Representation (IR) |
| 6  | Backpatching and Forward References |
| 7  | Virtual Machine Architecture |
| 8  | Linkers and Loaders |
| 9  | Operating System Essentials |
| 10 | Bringing It All Together |

Each chapter contains 3-4 sections with detailed technical content, code examples, and ASCII diagrams.

---

## Architecture

```
┌─────────────────┐
│  main.c          │  Parses CLI args, decides mode
│  Entry Point     │  (interactive / direct / export)
└────────┬────────┘
         │
    ┌────▼────┐
    │ reader.c│  Interactive CLI loop
    │         │  Text wrapping (72 cols)
    │         │  Export: text + HTML
    └────┬────┘
         │
    ┌────▼────────┐
    │ book_content.c│  load_book() populates
    │              │  10 chapters with content
    │              │  ASCII art, code snippets
    └──────────────┘
         │
    ┌────▼────┐
    │ spbook.h │  Shared structs:
    │          │  Section, Chapter, constants
    └──────────┘
```

The book content lives inside the program itself. `book_content.c` defines every chapter, section, and paragraph as C data structures. This makes the program its own documentation -- a self-contained, executable book.

---

## Chapter Overview

### Chapter 1: What Is System Programming?
Definition, scope, the von Neumann execution stack, and why we wrote a book that IS a program.

### Chapter 2: The C Language for Systems
Why C dominates the bottom of the stack. Memory layout (.text, .data, .bss, heap, stack). Undefined behavior. The four-stage compilation pipeline (preprocessor, compiler, assembler, linker).

### Chapter 3: Lexical Analysis
How a lexer converts raw text into a token stream. Token representation. Comment handling (`//` and `/* */`). Numeric and identifier scanning. The `advance_token()` loop.

### Chapter 4: Recursive-Descent Parsing
Grammars and parse trees. Operator precedence encoded through grammar nesting. Statement parsing (if, while, assign, return). Function definitions and parameter binding. LL(1) single-token lookahead.

### Chapter 5: Intermediate Representation (IR)
Why compilers use IRs. Three-Address Code (TAC) and quadruples. Temporary variable generation. Control-flow IR: conditional jumps (JMP_FALSE) and unconditional jumps (JMP). The `IR_Instr` struct.

### Chapter 6: Backpatching and Forward References
The forward-reference problem in single-pass compilers. Placeholder emission (`?`). Post-parse patching using saved array indices. While-loop backpatching (two patches: exit + loop-back). Backpatching lists in production compilers.

### Chapter 7: Virtual Machine Architecture
Stack machines vs. register machines (JVM vs. x86). The fetch-decode-execute loop. Call stack and `ExecutionFrame` struct. Variable resolution via `read_var()` / `write_var()`. Frame 0 as global scope.

### Chapter 8: Linkers and Loaders
From .o to executable: symbol resolution, relocation, section merging. The ELF format anatomy (header, segments, sections). Static vs. dynamic linking: GOT, PLT, lazy binding. The kernel loader: execve() demand paging.

### Chapter 9: Operating System Essentials
System calls: user/kernel boundary. Process management: PID, fork, execve, CFS scheduler. Virtual memory: page tables, TLB, copy-on-write. File systems: inodes, superblocks, VFS layer.

### Chapter 10: Bringing It All Together
The complete compiler pipeline from Chapters 3-7. What it takes to go from a toy compiler to Clang/GCC (AST, SSA, register allocation). System programming as a layered discipline.

---

## Companion Demos

Each demo is a standalone C program that illustrates a concept from the book:

| File | Concept | Chapters |
|------|---------|----------|
| `demo_memory.c` | Memory layout, section addresses | 2 |
| `demo_stack.c` | Call stack frames, local variables | 2, 7 |
| `demo_vm.c` | Mini stack-machine VM with bytecode loop | 7 |
| `demo_lexer.c` | Standalone lexer tokenizing an expression | 3 |

---

## Export Formats

### Text (`./spbook --text` or `e` in interactive mode)
Produces `spbook.txt` -- a plain-text version of the full book with chapter headings and section separators.

### HTML (`./spbook --html` or `h` in interactive mode)
Produces `spbook.html` -- a styled HTML page with:
- Responsive layout (max-width 800px)
- Table of Contents with anchor links
- Color-coded headings and code blocks
- Ready to host on GitHub Pages

---

## Design Philosophy

**The code IS the book.** All 10 chapters and 40 sections exist as C string arrays in `book_content.c`. There are no external data files. The program parses itself.

**Literate programming.** Every concept in the book has a runnable demo. Theory and practice are inseparable.

**Extensible.** Adding a new chapter means defining a `Chapter` struct in `load_book()` and calling `sec()` / `p()`. Adding a new export format means writing one function in `reader.c`.

---

## Build Everything

```bash
# Book reader
gcc main.c book_content.c reader.c -o spbook

# All demos
gcc demo_memory.c -o demo_memory
gcc demo_stack.c  -o demo_stack
gcc demo_vm.c     -o demo_vm
gcc demo_lexer.c  -o demo_lexer

# Run everything
./spbook --toc && echo "---" && \
./demo_memory && echo "---" && \
./demo_stack  && echo "---" && \
./demo_vm     && echo "---" && \
./demo_lexer
```
