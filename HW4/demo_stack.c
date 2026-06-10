// demo_stack.c -- Demonstrates call stack frames (Chapter 2, 7)
// Compile: gcc demo_stack.c -o demo_stack
// Run:     ./demo_stack

#include <stdio.h>

void frame_c(int depth) {
    int local_c = 30;
    printf("  Frame %d  |  local_c addr: %p  value: %d\n",
           depth, (void*)&local_c, local_c);
}

void frame_b(int depth) {
    int local_b = 20;
    printf("  Frame %d  |  local_b addr: %p  value: %d\n",
           depth, (void*)&local_b, local_b);
    frame_c(depth + 1);
}

void frame_a() {
    int local_a = 10;
    printf("  Frame %d  |  local_a addr: %p  value: %d\n",
           1, (void*)&local_a, local_a);
    frame_b(2);
}

int main() {
    printf("Call stack demonstration\n");
    printf("========================\n\n");
    printf("  Each function call pushes a new stack frame.\n");
    printf("  Local variables live in their frame.\n\n");
    frame_a();
    printf("\n  All frames popped -- back to main.\n");
    return 0;
}
