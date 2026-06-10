// demo_vm.c -- Standalone tiny stack-machine VM (Chapter 7 concept)
// Compile: gcc demo_vm.c -o demo_vm
// Run:     ./demo_vm

#include <stdio.h>
#include <stdlib.h>

typedef enum { OP_PUSH, OP_ADD, OP_SUB, OP_MUL, OP_PRINT, OP_HALT } Opcode;

typedef struct { Opcode op; int val; } Instr;

Instr program[] = {
    { OP_PUSH, 5 },
    { OP_PUSH, 3 },
    { OP_ADD,  0 },
    { OP_PUSH, 2 },
    { OP_MUL,  0 },
    { OP_PRINT, 0 },
    { OP_HALT, 0 },
};

void run_vm(Instr *prog, int len) {
    int stack[256];
    int sp = 0;
    int pc = 0;

    printf("Mini Stack VM -- computing (5 + 3) * 2\n\n");

    while (pc < len) {
        Instr ins = prog[pc];
        switch (ins.op) {
            case OP_PUSH:
                stack[sp++] = ins.val;
                printf("  PUSH %d  (stack top now: %d)\n", ins.val, stack[sp-1]);
                break;
            case OP_ADD:
                stack[sp-2] = stack[sp-2] + stack[sp-1];
                sp--;
                printf("  ADD     (stack top now: %d)\n", stack[sp-1]);
                break;
            case OP_SUB:
                stack[sp-2] = stack[sp-2] - stack[sp-1];
                sp--;
                printf("  SUB     (stack top now: %d)\n", stack[sp-1]);
                break;
            case OP_MUL:
                stack[sp-2] = stack[sp-2] * stack[sp-1];
                sp--;
                printf("  MUL     (stack top now: %d)\n", stack[sp-1]);
                break;
            case OP_PRINT:
                printf("\n  >> Result: %d\n", stack[sp-1]);
                break;
            case OP_HALT:
                printf("  HALT\n");
                return;
        }
        pc++;
    }
}

int main() {
    int instr_count = sizeof(program) / sizeof(program[0]);
    run_vm(program, instr_count);
    return 0;
}
