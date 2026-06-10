# Custom C Compiler & Virtual Machine

This repository contains a lightweight compiler and a stack-based Virtual Machine (VM) written entirely in C. It takes source code written in a custom procedural language, compiles it into Intermediate Representation (IR) quadruples, and executes it in an isolated VM environment.

## 🚀 Features

* **Lexical Analyzer (Lexer):** Tokenizes source code, handles whitespace, and ignores comments (`//` and `/* */`).
* **Recursive Descent Parser:** Processes tokens according to EBNF grammar rules.
* **Intermediate Representation (IR):** Flattens complex expressions into simple Three-Address Code / Quadruples (Opcode, Arg1, Arg2, Result).
* **Control Flow via Backpatching:** Fully supports `if` statements and `while` loops by dynamically patching jump addresses during compilation.
* **Function Calls & Stack Frames:** Supports defining and calling functions with arguments and return values using isolated execution frames.
* **Stack-Based Virtual Machine:** Simulates CPU execution by interpreting the generated IR line-by-line.

---

## 🛠️ Getting Started

### Prerequisites
You need a C compiler (like GCC or Clang) installed on your system.

### Compilation
To compile the compiler itself, open your terminal and run:

```bash
gcc compiler.c -o compiler

Execution
Create a text file containing your source code (e.g., program.txt), and then pass it to the compiled executable:
```bash
./compiler program.txt

💻 Language Syntax & Example
The custom language supports basic arithmetic, logical comparisons, variable assignments, loops, conditionals, and functions.

Here is an example of what program.txt might look like:
```bash
// Define a function
func multiply(a, b) {
    return a * b;
}

// Variables and Loop
x = 5;
result = 1;

while (x > 0) {
    result = multiply(result, x);
    x = x - 1;
} ```

🧠 Architecture Highlights
1. The while Loop Design (Backpatching)
Because the compiler reads code from top to bottom, it uses a technique called Backpatching to handle loops.
When it encounters a while statement, it generates a JMP_FALSE instruction with a placeholder destination (?). Once the parser finishes reading the entire body of the loop, it knows the exact memory address where the loop ends and goes back to "patch" the placeholder with the correct jump address.

2. Function Call Mechanism
To support recursion and prevent variables from clashing, the VM uses Execution Frames (Call Stack).

CALL: When a function is called, the VM pauses the current context, pushes a new frame onto the stack, and passes the arguments.

RETURN: Once the function finishes, the result is saved, the current frame is destroyed (freeing local variables), and the Virtual Machine jumps back to the exact instruction where the function was originally called.