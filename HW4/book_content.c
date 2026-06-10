#include "spbook.h"

Chapter book[MAX_CHAPTERS];
int total_chapters = 0;

static void s(Chapter *ch, const char *title) {
    ch->num_sections = 0;
    strcpy(ch->title, title);
}

static void p(Section *sec, const char *text) {
    int i = sec->num_lines;
    strncpy(sec->lines[i], text, MAX_LINE_LEN - 1);
    sec->lines[i][MAX_LINE_LEN - 1] = '\0';
    sec->num_lines++;
}

static void sec(Chapter *ch, const char *title) {
    int idx = ch->num_sections++;
    strcpy(ch->sections[idx].title, title);
    ch->sections[idx].num_lines = 0;
}

void load_book() {
    total_chapters = 0;
    Chapter *c;

    /* ==========================================================
       Chapter 1
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 1;
    s(c, "What Is System Programming?");

    sec(c, "1.1 Definition and Scope");
    p(&c->sections[0], "");
    p(&c->sections[0], "System programming is the craft of building software that sits");
    p(&c->sections[0], "close to the hardware: operating systems, device drivers, firmware,");
    p(&c->sections[0], "compilers, linkers, virtual machines, and embedded controllers.");
    p(&c->sections[0], "Unlike application programming---which uses services the OS already");
    p(&c->sections[0], "provides---system programming often creates those services.");
    p(&c->sections[0], "");
    p(&c->sections[0], "The central concern is resource management: CPU cycles, memory pages,");
    p(&c->sections[0], "disk blocks, network buffers.  A system programmer must understand");
    p(&c->sections[0], "the machine well enough to make guarantees about latency, throughput,");
    p(&c->sections[0], "and correctness even when failures occur.");
    p(&c->sections[0], "");
    p(&c->sections[0], "Languages of choice include C, C++, Rust, and increasingly Zig---all");
    p(&c->sections[0], "of which expose pointers, manual memory control, and the ability to");
    p(&c->sections[0], "embed assembly where needed.");

    sec(c, "1.2 The Execution Stack");
    p(&c->sections[1], "");
    p(&c->sections[1], "Every general-purpose computer today follows the von Neumann");
    p(&c->sections[1], "architecture: instructions and data share a single address space.");
    p(&c->sections[1], "Programs live as sequences of machine-code bytes; the CPU fetches,");
    p(&c->sections[1], "decodes, executes, and writes back in a continuous loop.");
    p(&c->sections[1], "");
    p(&c->sections[1], "   Application Layer         <-- your program");
    p(&c->sections[1], "   System Call Interface     <-- boundary to kernel");
    p(&c->sections[1], "   Kernel Layer              <-- scheduler, memory manager, IPC");
    p(&c->sections[1], "   Hardware Abstraction      <-- drivers map kernel ops to hardware");
    p(&c->sections[1], "");
    p(&c->sections[1], "In this book we will walk down the stack from the application toward");
    p(&c->sections[1], "the hardware, building a compiler and virtual machine along the way.");

    sec(c, "1.3 Why Write a Book with a Program?");
    p(&c->sections[2], "");
    p(&c->sections[2], "This book IS a program. You are reading it through a CLI reader that");
    p(&c->sections[2], "parses structured chapter data and renders it at your terminal.  But");
    p(&c->sections[2], "beyond the reader itself, every concept in the book is accompanied by");
    p(&c->sections[2], "a WORKING code example you can compile and run.  The compiler and VM");
    p(&c->sections[2], "developed in Chapters 3-4 serve as the living demonstration of");
    p(&c->sections[2], "parsing, code generation, and stack-machine execution.");
    p(&c->sections[2], "");
    p(&c->sections[2], "This 'literate programming' approach ensures that nothing is magic:");
    p(&c->sections[2], "you see the theory, you see the code, and you see the output.");

    /* ==========================================================
       Chapter 2
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 2;
    s(c, "The C Language for Systems");

    sec(c, "2.1 Why C Still Rules the Bottom");
    p(&c->sections[0], "");
    p(&c->sections[0], "C was designed in 1972 alongside Unix, and the two co-evolved.");
    p(&c->sections[0], "Today the Linux kernel, Windows kernel, FreeBSD, PostgreSQL,");
    p(&c->sections[0], "Redis, NGINX, and CPython are all written in C.  The language");
    p(&c->sections[0], "gives you three things that higher-level languages hide:");
    p(&c->sections[0], "");
    p(&c->sections[0], "  1. Raw pointers and pointer arithmetic.");
    p(&c->sections[0], "  2. Manual memory allocation (malloc / free).");
    p(&c->sections[0], "  3. Direct access to the system-call ABI.");
    p(&c->sections[0], "");
    p(&c->sections[0], "These are also the three things that make C dangerous.  The");
    p(&c->sections[0], "programmer, not the runtime, is responsible for safety.");

    sec(c, "2.2 Memory Layout of a C Program");
    p(&c->sections[1], "");
    p(&c->sections[1], "When the OS loads an ELF binary, memory is laid out like this:");
    p(&c->sections[1], "");
    p(&c->sections[1], "   High addresses");
    p(&c->sections[1], "   +-------------------+");
    p(&c->sections[1], "   |     Stack         |  <-- local variables, grows downward");
    p(&c->sections[1], "   |       |           |");
    p(&c->sections[1], "   |       v           |");
    p(&c->sections[1], "   |                   |");
    p(&c->sections[1], "   |       ^           |");
    p(&c->sections[1], "   |       |           |");
    p(&c->sections[1], "   |     Heap          |  <-- malloc'd memory, grows upward");
    p(&c->sections[1], "   +-------------------+");
    p(&c->sections[1], "   |   .bss (zeroed)   |  <-- uninitialized globals");
    p(&c->sections[1], "   |   .data (inited)  |  <-- initialized globals");
    p(&c->sections[1], "   |   .rodata         |  <-- string literals, constants");
    p(&c->sections[1], "   |   .text (code)    |  <-- executable instructions");
    p(&c->sections[1], "   +-------------------+");
    p(&c->sections[1], "   Low addresses");
    p(&c->sections[1], "");
    p(&c->sections[1], "Stack frames are pushed per function call, holding return");
    p(&c->sections[1], "addresses and local variables.  The heap is a free-for-all");
    p(&c->sections[1], "managed by malloc/free (or mmap for large allocations).");

    sec(c, "2.3 Undefined Behavior and How to Avoid It");
    p(&c->sections[2], "");
    p(&c->sections[2], "C has over 200 kinds of undefined behavior (UB).  The compiler");
    p(&c->sections[2], "may assume UB never happens and optimize accordingly.");
    p(&c->sections[2], "");
    p(&c->sections[2], "Common UB pitfalls:");
    p(&c->sections[2], "  - Signed integer overflow  (x = INT_MAX + 1)");
    p(&c->sections[2], "  - Out-of-bounds array access");
    p(&c->sections[2], "  - Use-after-free / double free");
    p(&c->sections[2], "  - Dereferencing NULL or a dangling pointer");
    p(&c->sections[2], "  - Aliasing violations (strict aliasing rule)");
    p(&c->sections[2], "");
    p(&c->sections[2], "Tools: -fsanitize=undefined,address (GCC/Clang) and Valgrind.");

    sec(c, "2.4 The Preprocessor, Compiler, Assembler, Linker");
    p(&c->sections[3], "");
    p(&c->sections[3], "A .c file goes through four stages:");
    p(&c->sections[3], "");
    p(&c->sections[3], "  1. Preprocessor (cpp):  expands #include, #define, #ifdef");
    p(&c->sections[3], "  2. Compiler (cc1):      C -> assembly (.s)");
    p(&c->sections[3], "  3. Assembler (as):      assembly -> object file (.o)");
    p(&c->sections[3], "  4. Linker (ld):         merges .o files -> executable (ELF)");
    p(&c->sections[3], "");
    p(&c->sections[3], "GCC and Clang let you stop at any stage:");
    p(&c->sections[3], "  $ gcc -E foo.c          # preprocess only");
    p(&c->sections[3], "  $ gcc -S foo.c          # compile to assembly");
    p(&c->sections[3], "  $ gcc -c foo.c          # assemble to object file");
    p(&c->sections[3], "  $ gcc foo.o -o foo      # link");

    /* ==========================================================
       Chapter 3
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 3;
    s(c, "Lexical Analysis");

    sec(c, "3.1 What a Lexer Does");
    p(&c->sections[0], "");
    p(&c->sections[0], "A lexer (scanner, tokenizer) is the front door of any compiler.");
    p(&c->sections[0], "It reads a raw stream of characters and groups them into tokens:");
    p(&c->sections[0], "the 'words' of a programming language.");
    p(&c->sections[0], "");
    p(&c->sections[0], "   Source:   counter = counter + 1;");
    p(&c->sections[0], "   Tokens:   ID(counter)  ASSIGN  ID(counter)  ADD  NUM(1)  SEMI");
    p(&c->sections[0], "");
    p(&c->sections[0], "The lexer discards whitespace and comments.  It does NOT care about");
    p(&c->sections[0], "syntax correctness---that is the parser's job.  Its only job is to");
    p(&c->sections[0], "produce a clean token stream.");

    sec(c, "3.2 Token Representation");
    p(&c->sections[1], "");
    p(&c->sections[1], "We represent each token as a (kind, lexeme) pair:");
    p(&c->sections[1], "");
    p(&c->sections[1], "   typedef enum { TOK_FUNC, TOK_RET, TOK_IF, ..., TOK_EOF } TokenKind;");
    p(&c->sections[1], "");
    p(&c->sections[1], "   typedef struct {");
    p(&c->sections[1], "       TokenKind kind;     // what kind of token");
    p(&c->sections[1], "       char lexeme[32];    // the actual text");
    p(&c->sections[1], "   } Token;");
    p(&c->sections[1], "");
    p(&c->sections[1], "Keywords (func, return, if, while) are recognized by string");
    p(&c->sections[1], "comparison after an identifier is read.  This is called 'keyword");
    p(&c->sections[1], "hashing' or 'reserved-word recognition.'");

    sec(c, "3.3 Handling Comments and Whitespace");
    p(&c->sections[2], "");
    p(&c->sections[2], "Comments come in two flavors:");
    p(&c->sections[2], "");
    p(&c->sections[2], "   // single-line comment -- skip until newline");
    p(&c->sections[2], "   /* multi-line");
    p(&c->sections[2], "      comment       */ -- skip until */");
    p(&c->sections[2], "");
    p(&c->sections[2], "Our lexer function advance_token() is called in a loop.  It first");
    p(&c->sections[2], "skips whitespace with isspace(), then checks for '/' to detect");
    p(&c->sections[2], "comments and advances the source pointer past the comment body.");
    p(&c->sections[2], "");
    p(&c->sections[2], "The loop structure ensures that after skipping a comment we return");
    p(&c->sections[2], "to the top and try again until we hit a real token or EOF.");

    sec(c, "3.4 Numeric and Identifier Scanning");
    p(&c->sections[3], "");
    p(&c->sections[3], "Numbers:   while (isdigit(*src)) buf[i++] = *src++;");
    p(&c->sections[3], "IDs:       while (isalnum(*src)) buf[i++] = *src++;");
    p(&c->sections[3], "");
    p(&c->sections[3], "After reading an ID, we compare against the keyword table.  If there");
    p(&c->sections[3], "is no match, the token is TOK_ID (user identifier).  The lexeme is");
    p(&c->sections[3], "stored so the parser can look at names for semantic decisions.");
    p(&c->sections[3], "");
    p(&c->sections[3], "Symbols like + - * / ( ) { } , ; < > are recognized via a switch");
    p(&c->sections[3], "statement.  The special case is '=' which can be either ASSIGN (=)");
    p(&c->sections[3], "or EQ (==) depending on whether the next character is also '='.");

    /* ==========================================================
       Chapter 4
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 4;
    s(c, "Recursive-Descent Parsing");

    sec(c, "4.1 Grammars and Parse Trees");
    p(&c->sections[0], "");
    p(&c->sections[0], "A grammar is a set of rules that defines the syntax of a language.");
    p(&c->sections[0], "For example, an expression grammar:");
    p(&c->sections[0], "");
    p(&c->sections[0], "   expr  -> term (('+'|'-') term)*");
    p(&c->sections[0], "   term  -> factor (('*'|'/') factor)*");
    p(&c->sections[0], "   factor-> NUM | ID | '(' expr ')'");
    p(&c->sections[0], "");
    p(&c->sections[0], "Each rule corresponds to a function in a recursive-descent parser.");
    p(&c->sections[0], "The parser 'descends' from the start symbol (program) through");
    p(&c->sections[0], "intermediate symbols (statement, expression, term, factor) until");
    p(&c->sections[0], "terminal tokens are consumed.");

    sec(c, "4.2 Operator Precedence Through Grammar Structure");
    p(&c->sections[1], "");
    p(&c->sections[1], "Precedence is encoded by the nesting of grammar rules:");
    p(&c->sections[1], "");
    p(&c->sections[1], "   parse_expression()       // handles ==, <, >   (lowest prec)");
    p(&c->sections[1], "     -> parse_math_expr()   // handles +, -");
    p(&c->sections[1], "        -> parse_term()     // handles *, /");
    p(&c->sections[1], "           -> parse_factor()// handles NUM, ID, (expr), func()");
    p(&c->sections[1], "");
    p(&c->sections[1], "Higher-precedence operators are 'deeper' in the call chain.");
    p(&c->sections[1], "Associativity is determined by the WHILE loop direction: left-to-");
    p(&c->sections[1], "right evaluation within each level creates left-associativity.");

    sec(c, "4.3 Statement Parsing: if, while, assignments");
    p(&c->sections[2], "");
    p(&c->sections[2], "Our parser handles four statement types:");
    p(&c->sections[2], "");
    p(&c->sections[2], "  1. if (cond) { body }");
    p(&c->sections[2], "  2. while (cond) { body }");
    p(&c->sections[2], "  3. identifier = expression ;");
    p(&c->sections[2], "  4. return expression ;");
    p(&c->sections[2], "");
    p(&c->sections[2], "Each is a branch inside parse_statement().  The parser uses a");
    p(&c->sections[2], "single-token lookahead (current_token) to decide which production");
    p(&c->sections[2], "to apply.  This makes it an LL(1) parser in most productions.");

    sec(c, "4.4 Function Definitions and Parameter Binding");
    p(&c->sections[3], "");
    p(&c->sections[3], "Function parsing happens at the top level in compile_program():");
    p(&c->sections[3], "");
    p(&c->sections[3], "   func name(param1, param2) { body }");
    p(&c->sections[3], "");
    p(&c->sections[3], "The function keyword triggers DEF_FUNC IR emission.  Parameters are");
    p(&c->sections[3], "bound via BIND_PARAM instructions.  The body is a sequence of");
    p(&c->sections[3], "statements parsed recursively.  At the closing brace, END_FUNC is");
    p(&c->sections[3], "emitted to signal the VM to return control or stop skipping.");

    /* ==========================================================
       Chapter 5
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 5;
    s(c, "Intermediate Representation (IR)");

    sec(c, "5.1 Why Not Go Straight to Machine Code?");
    p(&c->sections[0], "");
    p(&c->sections[0], "Real compilers rarely emit machine code directly.  They use an IR:");
    p(&c->sections[0], "");
    p(&c->sections[0], "  Source -> Tokens -> AST -> IR -> Optimized IR -> Machine Code");
    p(&c->sections[0], "");
    p(&c->sections[0], "Benefits of an IR:");
    p(&c->sections[0], "  - Machine-independent optimizations (constant folding, DCE)");
    p(&c->sections[0], "  - Multiple back-ends (x86, ARM, RISC-V) from one front-end");
    p(&c->sections[0], "  - Easier debugging: IR is readable by humans");
    p(&c->sections[0], "  - Separation of concerns: parser knows grammar, codegen knows ops");

    sec(c, "5.2 Three-Address Code (TAC / Quadruples)");
    p(&c->sections[1], "");
    p(&c->sections[1], "Three-address code represents each operation as (op, src1, src2, dst):");
    p(&c->sections[1], "");
    p(&c->sections[1], "   x = a + b * c");
    p(&c->sections[1], "   becomes:");
    p(&c->sections[1], "   000: MUL     b     c     t1");
    p(&c->sections[1], "   001: ADD     a     t1    t2");
    p(&c->sections[1], "   002: STORE   t2    -     x");
    p(&c->sections[1], "");
    p(&c->sections[1], "Our IR structure stores 4 strings per instruction:");
    p(&c->sections[1], "");
    p(&c->sections[1], "   typedef struct {");
    p(&c->sections[1], "       char opcode[16];   // ADD, SUB, CALL, JMP, ...");
    p(&c->sections[1], "       char arg1[32];     // first operand");
    p(&c->sections[1], "       char arg2[32];     // second operand");
    p(&c->sections[1], "       char result[32];   // destination or jump target");
    p(&c->sections[1], "   } IR_Instr;");

    sec(c, "5.3 Temporary Variables and Register Pressure");
    p(&c->sections[2], "");
    p(&c->sections[2], "Each sub-expression result gets a unique temporary name (t1, t2, ...)");
    p(&c->sections[2], "via generate_temp().  This is the simplest form of register allocation:");
    p(&c->sections[2], "every value gets its own 'register'.");
    p(&c->sections[2], "");
    p(&c->sections[2], "In a production compiler this would be followed by a register allocator");
    p(&c->sections[2], "that maps unlimited temporaries to a fixed set of machine registers");
    p(&c->sections[2], "using graph-coloring or linear-scan algorithms.  Spilled values go to");
    p(&c->sections[2], "the stack.  Our VM sidesteps this by treating everything as named vars.");

    sec(c, "5.4 Control-Flow IR: Jumps and Labels");
    p(&c->sections[3], "");
    p(&c->sections[3], "Conditional logic requires non-linear execution:");
    p(&c->sections[3], "");
    p(&c->sections[3], "   if (x > 0) {");
    p(&c->sections[3], "       y = 1;");
    p(&c->sections[3], "   }");
    p(&c->sections[3], "");
    p(&c->sections[3], "Becomes:");
    p(&c->sections[3], "   000: IS_GT    x     0     t1");
    p(&c->sections[3], "   001: JMP_FALSE t1   -     4    // skip to 004 if false");
    p(&c->sections[3], "   002: LOAD_IMM  1    -     t2");
    p(&c->sections[3], "   003: STORE    t2    -     y");
    p(&c->sections[3], "   004: ...                         // continuation");
    p(&c->sections[3], "");
    p(&c->sections[3], "JMP_FALSE tests its condition argument; if zero, jumps to the PC");
    p(&c->sections[3], "stored in its result field.  This implements structured if/while on");
    p(&c->sections[3], "top of unstructured goto semantics at the IR level.");

    /* ==========================================================
       Chapter 6
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 6;
    s(c, "Backpatching and Forward References");

    sec(c, "6.1 The Forward-Reference Problem");
    p(&c->sections[0], "");
    p(&c->sections[0], "When parsing 'if (cond) { body }', the JMP_FALSE instruction must");
    p(&c->sections[0], "jump PAST the body to the instruction after the block.  But when");
    p(&c->sections[0], "JMP_FALSE is first emitted, we do not yet know how many instructions");
    p(&c->sections[0], "the body will produce---so we do not know the target address.");
    p(&c->sections[0], "");
    p(&c->sections[0], "This is a fundamental single-pass compiler problem.  Solutions:");
    p(&c->sections[0], "  1. Two-pass: build full AST first, then emit (needs more memory).");
    p(&c->sections[0], "  2. Backpatching: emit a placeholder, fix it later.");
    p(&c->sections[0], "  3. Peg-hole optimization: always emit worst-case padding then fill.");

    sec(c, "6.2 Placeholder Emission");
    p(&c->sections[1], "");
    p(&c->sections[1], "Our strategy:");
    p(&c->sections[1], "");
    p(&c->sections[1], "  int jump_f_loc = ir_index;                 // remember where");
    p(&c->sections[1], "  emit_ir(\"JMP_FALSE\", condition, \"-\", \"?\");  // '?' = placeholder");
    p(&c->sections[1], "");
    p(&c->sections[1], "The '?' in the result field signals 'not yet known.'  We save the");
    p(&c->sections[1], "array index (jump_f_loc) so we can patch it later.");

    sec(c, "6.3 Post-Parse Patching");
    p(&c->sections[2], "");
    p(&c->sections[2], "Once the body has been fully parsed (closing '}'), the current");
    p(&c->sections[2], "ir_index tells us exactly where the next instruction will go:");
    p(&c->sections[2], "");
    p(&c->sections[2], "  sprintf(program_ir[jump_f_loc].result, \"%d\", ir_index);");
    p(&c->sections[2], "");
    p(&c->sections[2], "Now the JMP_FALSE from earlier points to the correct target.");
    p(&c->sections[2], "No second pass needed---we patched it in place.");
    p(&c->sections[2], "");
    p(&c->sections[2], "For while loops, TWO patches are needed:");
    p(&c->sections[2], "  - The JMP_FALSE (exit) is patched to point past the loop.");
    p(&c->sections[2], "  - The unconditional JMP at the end is patched back to loop_start_pc.");

    sec(c, "6.4 Limitations and Next Steps");
    p(&c->sections[3], "");
    p(&c->sections[3], "Backpatching works well for structured control flow but struggles");
    p(&c->sections[3], "with complex features like break, continue, or goto.  For those, a");
    p(&c->sections[3], "symbol table with label-to-address mappings is cleaner.");
    p(&c->sections[3], "");
    p(&c->sections[3], "The backpatching list technique (used in real compilers like GCC's");
    p(&c->sections[3], "early versions) maintains linked lists of instructions that need");
    p(&c->sections[3], "the same target, patching them all at once when the target becomes");
    p(&c->sections[3], "known.  This is more powerful than our simple single-slot approach.");

    /* ==========================================================
       Chapter 7
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 7;
    s(c, "Virtual Machine Architecture");

    sec(c, "7.1 Stack Machines vs. Register Machines");
    p(&c->sections[0], "");
    p(&c->sections[0], "Processor ISAs fall into two families:");
    p(&c->sections[0], "");
    p(&c->sections[0], "  Stack Machine (JVM, WebAssembly, Forth):");
    p(&c->sections[0], "    Operands live on an evaluation stack.  ADD pops two, pushes one.");
    p(&c->sections[0], "    Pro: dense encoding.  Con: more instructions per operation.");
    p(&c->sections[0], "");
    p(&c->sections[0], "  Register Machine (x86, ARM, RISC-V):");
    p(&c->sections[0], "    Operations specify registers directly: add r1, r2, r3.");
    p(&c->sections[0], "    Pro: maps well to hardware.  Con: larger instruction words.");
    p(&c->sections[0], "");
    p(&c->sections[0], "Our VM is a HYBRID: variables are name-addressed (not stack slots),");
    p(&c->sections[0], "but function arguments pass through an arg_buffer (stack-like).");

    sec(c, "7.2 The Execution Loop");
    p(&c->sections[1], "");
    p(&c->sections[1], "Every VM follows the same fetch-decode-execute cycle:");
    p(&c->sections[1], "");
    p(&c->sections[1], "   while (pc < ir_index) {");
    p(&c->sections[1], "       IR_Instr cmd = program_ir[pc];    // fetch");
    p(&c->sections[1], "       if (strcmp(cmd.opcode, \"ADD\") == 0) {");
    p(&c->sections[1], "           write_var(cmd.result, read_var(cmd.arg1) + read_var(cmd.arg2));");
    p(&c->sections[1], "       } else if (...) { ... }");
    p(&c->sections[1], "       pc++;                              // advance (or jump)");
    p(&c->sections[1], "   }");
    p(&c->sections[1], "");
    p(&c->sections[1], "Note that some instructions (JMP, CALL, RETURN) set pc explicitly");
    p(&c->sections[1], "and use 'continue' to skip the normal pc++ at the loop bottom.");
    p(&c->sections[1], "");
    p(&c->sections[1], "This is the heart of interpretation: a tight loop over an array of");
    p(&c->sections[1], "instructions, with a switch or if-else chain dispatching each opcode.");
    p(&c->sections[1], "Hardware CPUs do the same thing, just in silicon instead of C.");

    sec(c, "7.3 Call Stack and Stack Frames");
    p(&c->sections[2], "");
    p(&c->sections[2], "When a function is called, a new ExecutionFrame is pushed onto the");
    p(&c->sections[2], "call stack.  Each frame stores:");
    p(&c->sections[2], "");
    p(&c->sections[2], "  - A variable table (var_names[], var_values[]) for locals");
    p(&c->sections[2], "  - The return program counter (where to resume after RETURN)");
    p(&c->sections[2], "  - The target variable for the return value");
    p(&c->sections[2], "  - Passed arguments, received before BIND_PARAM binds them");
    p(&c->sections[2], "");
    p(&c->sections[2], "Frame 0 is special: it is the global scope, never popped.");
    p(&c->sections[2], "Each function call increments frame_ptr; each RETURN decrements it.");
    p(&c->sections[2], "");
    p(&c->sections[2], "The ExecutionFrame struct (in vm.c) is the foundational data");
    p(&c->sections[2], "structure of our VM---it's what threads usually provide in an OS,");
    p(&c->sections[2], "and what the JVM calls activation records.");

    sec(c, "7.4 Variable Resolution");
    p(&c->sections[3], "");
    p(&c->sections[3], "read_var(name) searches the CURRENT frame's variable table linearly.");
    p(&c->sections[3], "If the name looks like a number (isdigit), it returns the number");
    p(&c->sections[3], "directly---this is how literal values propagate through the VM.");
    p(&c->sections[3], "");
    p(&c->sections[3], "write_var(name, val) searches first for an existing entry to update;");
    p(&c->sections[3], "if not found, it appends a new entry.  This means variables are");
    p(&c->sections[3], "implicitly declared on first assignment---like Python but without");
    p(&c->sections[3], "a declaration keyword.");

    /* ==========================================================
       Chapter 8
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 8;
    s(c, "Linkers and Loaders");

    sec(c, "8.1 From Object File to Executable");
    p(&c->sections[0], "");
    p(&c->sections[0], "A linker takes one or more object files (.o) and produces a single");
    p(&c->sections[0], "executable or shared library.  Its main jobs:");
    p(&c->sections[0], "");
    p(&c->sections[0], "  Symbol resolution:");
    p(&c->sections[0], "    Match call sites to definitions.  Undefined externals become");
    p(&c->sections[0], "    errors.  Duplicate definitions are flagged.");
    p(&c->sections[0], "");
    p(&c->sections[0], "  Relocation:");
    p(&c->sections[0], "    Each .o file was compiled independently; its addresses start");
    p(&c->sections[0], "    from 0.  The linker assigns final addresses and patches every");
    p(&c->sections[0], "    instruction that references a relocated symbol.");
    p(&c->sections[0], "");
    p(&c->sections[0], "  Section merging:");
    p(&c->sections[0], "    All .text sections become one text segment.  All .data sections");
    p(&c->sections[0], "    become one data segment.  Padding is inserted for alignment.");

    sec(c, "8.2 The ELF Format");
    p(&c->sections[1], "");
    p(&c->sections[1], "ELF (Executable and Linkable Format) is the standard binary format");
    p(&c->sections[1], "on Linux, BSD, and many embedded systems.  Structure:");
    p(&c->sections[1], "");
    p(&c->sections[1], "  ELF Header: magic bytes (0x7F E L F), architecture, entry point");
    p(&c->sections[1], "  Program Header Table: describes segments for the loader");
    p(&c->sections[1], "  Sections: .text, .data, .bss, .rodata, .symtab, .strtab, ...");
    p(&c->sections[1], "  Section Header Table: metadata about each section");
    p(&c->sections[1], "");
    p(&c->sections[1], "Tools: readelf -h, objdump -d, nm, ldd, size.");

    sec(c, "8.3 Dynamic Linking");
    p(&c->sections[2], "");
    p(&c->sections[2], "Static linking copies all library code into the executable (big,");
    p(&c->sections[2], "self-contained).  Dynamic linking defers resolution to load time:");
    p(&c->sections[2], "");
    p(&c->sections[2], "  1. The dynamic linker (ld.so) is invoked by the kernel.");
    p(&c->sections[2], "  2. It loads shared libraries (.so) into the process address space.");
    p(&c->sections[2], "  3. It resolves symbols via the Global Offset Table (GOT) and");
    p(&c->sections[2], "     Procedure Linkage Table (PLT).");
    p(&c->sections[2], "  4. Lazy binding: symbols resolved on first call, not at load.");
    p(&c->sections[2], "");
    p(&c->sections[2], "This allows library updates without recompiling the application");

    sec(c, "8.4 The Loader: From Disk to Memory");
    p(&c->sections[3], "");
    p(&c->sections[3], "When you run a program, the kernel's execve() syscall:");
    p(&c->sections[3], "  1. Parses the ELF header to verify the format.");
    p(&c->sections[3], "  2. Creates a new virtual address space for the process.");
    p(&c->sections[3], "  3. Maps LOAD segments into memory (mmap with file backing).");
    p(&c->sections[3], "  4. Sets up the initial stack with argv, envp, and aux vectors.");
    p(&c->sections[3], "  5. Jumps to the entry point (_start, which calls __libc_start_main");
    p(&c->sections[3], "     and eventually main()).");
    p(&c->sections[3], "");
    p(&c->sections[3], "Demand paging: pages are loaded from disk only when accessed.");

    /* ==========================================================
       Chapter 9
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 9;
    s(c, "Operating System Essentials");

    sec(c, "9.1 System Calls: The User/Kernel Boundary");
    p(&c->sections[0], "");
    p(&c->sections[0], "User programs cannot touch hardware directly.  They request services");
    p(&c->sections[0], "from the kernel via system calls:");
    p(&c->sections[0], "");
    p(&c->sections[0], "  read(2)   - read bytes from a file descriptor");
    p(&c->sections[0], "  write(2)  - write bytes to a file descriptor");
    p(&c->sections[0], "  open(2)   - open a file");
    p(&c->sections[0], "  mmap(2)   - map memory");
    p(&c->sections[0], "  fork(2)   - create a child process");
    p(&c->sections[0], "  execve(2) - replace process image");
    p(&c->sections[0], "");
    p(&c->sections[0], "On x86 Linux, a syscall is triggered by:");
    p(&c->sections[0], "  mov eax, <syscall_number>  ; int 0x80");
    p(&c->sections[0], "  On x86-64 the 'syscall' instruction is preferred.");

    sec(c, "9.2 Process Management");
    p(&c->sections[1], "");
    p(&c->sections[1], "A process is a running program with its own virtual address space.");
    p(&c->sections[1], "Key properties:");
    p(&c->sections[1], "");
    p(&c->sections[1], "  PID:   unique process identifier");
    p(&c->sections[1], "  PPID:  parent's PID");
    p(&c->sections[1], "  State: running, sleeping, stopped, zombie");
    p(&c->sections[1], "");
    p(&c->sections[1], "Process creation: fork() duplicates the calling process.  The child");
    p(&c->sections[1], "gets a copy of the parent's address space (copy-on-write).  Often");
    p(&c->sections[1], "followed by execve() to replace the child with a new program.");
    p(&c->sections[1], "");
    p(&c->sections[1], "The kernel scheduler decides which process runs on each CPU core.");
    p(&c->sections[1], "Linux uses CFS (Completely Fair Scheduler): a red-black tree ordered");
    p(&c->sections[1], "by vruntime to give each task a fair share of CPU time.");

    sec(c, "9.3 Virtual Memory");
    p(&c->sections[2], "");
    p(&c->sections[2], "Virtual memory gives each process the illusion of having the entire");
    p(&c->sections[2], "address space to itself.  The MMU (Memory Management Unit) translates");
    p(&c->sections[2], "virtual addresses to physical addresses using page tables.");
    p(&c->sections[2], "");
    p(&c->sections[2], "  Page size: typically 4 KB (can be 2 MB or 1 GB with huge pages).");
    p(&c->sections[2], "  Page table levels: 4 on x86-64 (PGD -> PUD -> PMD -> PTE).");
    p(&c->sections[2], "  TLB: Translation Lookaside Buffer caches recent translations.");
    p(&c->sections[2], "");
    p(&c->sections[2], "Benefits: isolation (one process can't read another's memory),");
    p(&c->sections[2], "paging (inactive pages moved to disk), memory-mapped files.");

    sec(c, "9.4 File Systems");
    p(&c->sections[3], "");
    p(&c->sections[3], "A file system organizes bytes on a block device.  Key abstractions:");
    p(&c->sections[3], "");
    p(&c->sections[3], "  Inode: metadata about one file (permissions, size, block pointers)");
    p(&c->sections[3], "  Directory: mapping from names to inode numbers");
    p(&c->sections[3], "  Superblock: file system parameters (block size, inode count)");
    p(&c->sections[3], "");
    p(&c->sections[3], "Common Linux file systems: ext4, XFS, Btrfs, ZFS.");
    p(&c->sections[3], "The Virtual File System (VFS) layer provides a uniform interface");
    p(&c->sections[3], "so the kernel can support multiple file system types transparently.");

    /* ==========================================================
       Chapter 10
       ========================================================== */
    c = &book[total_chapters++];
    c->number = 10;
    s(c, "Bringing It All Together");

    sec(c, "10.1 The Complete Compiler Pipeline");
    p(&c->sections[0], "");
    p(&c->sections[0], "We have now covered every stage of a simple compiler:");
    p(&c->sections[0], "");
    p(&c->sections[0], "  1. Lexer (Chapter 3):        text -> tokens");
    p(&c->sections[0], "  2. Parser (Chapter 4):       tokens -> IR via recursive descent");
    p(&c->sections[0], "  3. IR Emission (Chapter 5):  TAC quadruples with temporaries");
    p(&c->sections[0], "  4. Backpatching (Chapter 6): fix forward jump references");
    p(&c->sections[0], "  5. VM Execution (Chapter 7): fetch-decode-execute on call stack");
    p(&c->sections[0], "");
    p(&c->sections[0], "This is a full, self-contained system that turns source code into");
    p(&c->sections[0], "running computation---the essence of what a computer does.");

    sec(c, "10.2 From Here to a Production Compiler");
    p(&c->sections[1], "");
    p(&c->sections[1], "To go from this toy to something like GCC or LLVM, you need:");
    p(&c->sections[1], "");
    p(&c->sections[1], "  - A proper AST (Abstract Syntax Tree) between parser and codegen");
    p(&c->sections[1], "  - Semantic analysis (type checking, scope resolution)");
    p(&c->sections[1], "  - An optimization pipeline (constant propagation, CSE, inlining)");
    p(&c->sections[1], "  - Static Single Assignment (SSA) form for advanced transforms");
    p(&c->sections[1], "  - Register allocation via graph coloring or linear scan");
    p(&c->sections[1], "  - Target-specific code generation and instruction scheduling");
    p(&c->sections[1], "  - A standard library and runtime system");
    p(&c->sections[1], "");
    p(&c->sections[1], "But the principles---tokenization, parsing, IR, VM execution---are");
    p(&c->sections[1], "the same whether you compile to machine code or bytecode.");

    sec(c, "10.3 The Bigger Picture: System Programming as a Discipline");
    p(&c->sections[2], "");
    p(&c->sections[2], "System programming is about understanding and controlling the");
    p(&c->sections[2], "layers of abstraction that separate your code from the hardware.");
    p(&c->sections[2], "");
    p(&c->sections[2], "  - Compilers translate between layers.");
    p(&c->sections[2], "  - Operating systems manage shared resources.");
    p(&c->sections[2], "  - Linkers and loaders stitch programs together.");
    p(&c->sections[2], "  - Virtual machines provide portability.");
    p(&c->sections[2], "");
    p(&c->sections[2], "Every modern software system---browsers, databases, game engines,");
    p(&c->sections[2], "cloud infrastructure---sits on these foundations.  Understanding");
    p(&c->sections[2], "them makes you a better engineer at every level of the stack.");
    p(&c->sections[2], "");
    p(&c->sections[2], "This book has been generated as a C program that you can read,");
    p(&c->sections[2], "modify, and extend.  The code IS the book, and the book IS the");
    p(&c->sections[2], "code.  We hope it inspires you to dig deeper into the machinery.");
    p(&c->sections[2], "");
    p(&c->sections[2], "                    --- End of Book ---");
}
