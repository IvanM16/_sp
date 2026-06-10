#include "tiny_compiler.h"

IR_Instr program_ir[1000];
int ir_index = 0;

void emit_ir(const char *op, const char *a1, const char *a2, const char *res) {
    strcpy(program_ir[ir_index].opcode, op);
    strcpy(program_ir[ir_index].arg1, a1);
    strcpy(program_ir[ir_index].arg2, a2);
    strcpy(program_ir[ir_index].result, res);
    
    // Print the generated IR line for debugging/visibility
    printf("%03d: %-10s %-10s %-10s %-10s\n", ir_index, op, a1, a2, res);
    ir_index++;
}