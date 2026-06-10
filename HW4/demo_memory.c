// demo_memory.c -- Demonstrates memory layout concepts from Chapter 2
// Compile: gcc demo_memory.c -o demo_memory
// Run:     ./demo_memory

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int global_init = 42;        // .data section
int global_zero;             // .bss section
const char *ro_string = "read-only";  // .rodata

void print_address(const char *label, void *ptr) {
    printf("  %-20s  %p\n", label, ptr);
}

int main() {
    int local = 7;                          // stack
    int *heap_chunk = malloc(sizeof(int));  // heap
    *heap_chunk = 99;

    printf("Memory Layout Demonstration\n");
    printf("===========================\n\n");

    print_address(".text (code)", (void*)print_address);
    print_address(".data (init global)", &global_init);
    print_address(".bss  (zero global)", &global_zero);
    print_address(".rodata (string)", (void*)ro_string);
    print_address("Stack (local var)", &local);
    print_address("Heap  (malloc)", heap_chunk);

    printf("\nObservations:\n");
    printf("  - Code (.text) is at the lowest address\n");
    printf("  - Data sections follow\n");
    printf("  - The heap is higher\n");
    printf("  - The stack is at the highest address\n");
    printf("  - Stack and heap grow toward each other\n");

    free(heap_chunk);
    return 0;
}
