# 🧩 Modular C-Compiler & Virtual Machine

> A lightweight, multi-file compiler and stack-based Virtual Machine built in C. This project demonstrates modular software architecture by separating the compilation pipeline into distinct, manageable components.

---

## 🌌 Overview

Unlike monolithic single-file compilers, this project is split into focused modules. It translates a custom high-level procedural language into **Three-Address Code (Quadruples)** and executes it seamlessly inside a custom-built **Virtual Machine (VM)**. 

### 🗂️ Project Structure

The project is divided into the following core components:

* 📄 **`tiny_compiler.h`** — The central header file containing shared definitions, structs, and function prototypes.
* 📄 **`lexer.c`** — The Lexical Analyzer. Reads raw source code and converts it into a stream of tokens.
* 📄 **`parser.c`** — The Recursive Descent Parser. Enforces grammar rules and structural logic.
* 📄 **`codegen.c`** — The Intermediate Representation (IR) Generator. Emits execution instructions (Quadruples).
* 📄 **`vm.c`** — The Virtual Machine. Simulates a CPU and execution stack to run the generated IR.
* 📄 **`main.c`** — The entry point. Handles file reading and orchestrates the compilation pipeline.

---

## ✨ Key Features

* **Architectural Modularity:** Clean separation of concerns making the codebase easy to read, debug, and expand.
* **Advanced Lexer:** Gracefully handles whitespaces, symbols, and skips single-line (`//`) or multi-line (`/* */`) comments.
* **Dynamic Backpatching:** Resolves forward-jump addresses for conditional statements (`if`) and loops (`while`) dynamically during the compilation phase.
* **Stack Frame Architecture:** The VM implements a localized runtime call stack allowing clean scope separation, parameter binding, and accurate recursive function returns.

---

## 🛠️ Quick Start

### 1. Compilation
Because the project is modular, you need to compile all the `.c` files together. Use a standard C compiler like GCC:

```bash
gcc main.c lexer.c parser.c codegen.c vm.c -o my_compiler
```

### 2. Running a Script
Create a source file (e.g., program.txt) with your code, and pass it to the compiled executable:
```
./my_compiler program.txt
```
## 📝 Syntax Showcase
The custom language balances structural simplicity with procedural power. Below is an example demonstrating variables, math, loops, and function calls:
```// A modular function to handle calculations
func compute_factor(base, multiplier) {
    return base * multiplier;
}

// Main execution block
counter = 5;
accumulator = 1;

while (counter > 0) {
    accumulator = compute_factor(accumulator, counter);
    counter = counter - 1;
}

// The VM will output the global state upon termination
print(accumulator);
```
## 🧠 Core Engineering Principles

### 🔄 The Backpatching Strategy
In a single-pass compiler, the exact destination of a jump instruction (like exiting a while loop) is unknown when the loop header is first parsed. This compiler handles this by emitting placeholder addresses (?) into the IR array. Once the inner statement body is completely parsed, the compiler safely traverses backward and patches the correct pointer locations.

### 📥 Virtual Machine Call Stack
To allow functions to co-exist without overriding each other's memory spaces, the Virtual Machine relies on an explicit ExecutionFrame stack:

1. Allocation: Calling a function pushes a new frame onto the stack.

2. Context Saving: The return address (PC + 1) and target return variables are safely cached.

3. Deallocation: Upon reaching a RETURN opcode, the current frame environment is completely popped, transferring control cleanly back to the caller context.