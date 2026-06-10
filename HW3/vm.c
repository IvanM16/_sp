#include "tiny_compiler.h"

typedef struct {
    char var_names[100][32];
    int var_values[100];
    int local_count;
    int return_pc;
    char target_var[32];
    int passed_args[10];
    int arg_index;
} ExecutionFrame;

ExecutionFrame call_stack[1000];
int frame_ptr = 0; 

int read_var(const char *name) {
    if (isdigit(name[0])) return atoi(name);
    for (int i = 0; i < call_stack[frame_ptr].local_count; i++) {
        if (strcmp(call_stack[frame_ptr].var_names[i], name) == 0)
            return call_stack[frame_ptr].var_values[i];
    }
    return 0;
}

void write_var(const char *name, int val) {
    for (int i = 0; i < call_stack[frame_ptr].local_count; i++) {
        if (strcmp(call_stack[frame_ptr].var_names[i], name) == 0) {
            call_stack[frame_ptr].var_values[i] = val;
            return;
        }
    }
    strcpy(call_stack[frame_ptr].var_names[call_stack[frame_ptr].local_count], name);
    call_stack[frame_ptr].var_values[call_stack[frame_ptr].local_count++] = val;
}

void execute_vm() {
    int pc = 0;
    int arg_buffer[1000];
    int buffer_ptr = 0;
    
    int func_addresses[100];
    char func_names[100][32];
    int total_funcs = 0;

    for (int i = 0; i < ir_index; i++) {
        if (strcmp(program_ir[i].opcode, "DEF_FUNC") == 0) {
            strcpy(func_names[total_funcs], program_ir[i].arg1);
            func_addresses[total_funcs++] = i + 1;
        }
    }

    call_stack[frame_ptr].local_count = 0;
    printf("\n=== VM Execution Started ===\n");

    while (pc < ir_index) {
        IR_Instr cmd = program_ir[pc];
        
        if (strcmp(cmd.opcode, "DEF_FUNC") == 0) {
            while (strcmp(program_ir[pc].opcode, "END_FUNC") != 0) pc++;
        } 
        else if (strcmp(cmd.opcode, "LOAD_IMM") == 0) write_var(cmd.result, atoi(cmd.arg1));
        else if (strcmp(cmd.opcode, "ADD") == 0) write_var(cmd.result, read_var(cmd.arg1) + read_var(cmd.arg2));
        else if (strcmp(cmd.opcode, "SUB") == 0) write_var(cmd.result, read_var(cmd.arg1) - read_var(cmd.arg2));
        else if (strcmp(cmd.opcode, "MUL") == 0) write_var(cmd.result, read_var(cmd.arg1) * read_var(cmd.arg2));
        else if (strcmp(cmd.opcode, "DIV") == 0) write_var(cmd.result, read_var(cmd.arg1) / read_var(cmd.arg2));
        else if (strcmp(cmd.opcode, "IS_EQ") == 0) write_var(cmd.result, read_var(cmd.arg1) == read_var(cmd.arg2));
        else if (strcmp(cmd.opcode, "IS_LT") == 0) write_var(cmd.result, read_var(cmd.arg1) < read_var(cmd.arg2));
        else if (strcmp(cmd.opcode, "IS_GT") == 0) write_var(cmd.result, read_var(cmd.arg1) > read_var(cmd.arg2));
        else if (strcmp(cmd.opcode, "STORE") == 0) write_var(cmd.result, read_var(cmd.arg1));
        
        else if (strcmp(cmd.opcode, "JMP_FALSE") == 0) {
            if (read_var(cmd.arg1) == 0) pc = atoi(cmd.result) - 1;
        } 
        else if (strcmp(cmd.opcode, "JMP") == 0) {
            pc = atoi(cmd.result) - 1;
        } 
        else if (strcmp(cmd.opcode, "PUSH_ARG") == 0) {
            arg_buffer[buffer_ptr++] = read_var(cmd.arg1);
        } 
        else if (strcmp(cmd.opcode, "CALL") == 0) {
            int args_to_pass = atoi(cmd.arg2);
            int target_address = -1;
            for (int i = 0; i < total_funcs; i++) {
                if (strcmp(func_names[i], cmd.arg1) == 0) target_address = func_addresses[i];
            }

            frame_ptr++; 
            call_stack[frame_ptr].local_count = 0;
            call_stack[frame_ptr].return_pc = pc + 1;
            strcpy(call_stack[frame_ptr].target_var, cmd.result);
            call_stack[frame_ptr].arg_index = 0;
            
            for (int i = 0; i < args_to_pass; i++) {
                call_stack[frame_ptr].passed_args[i] = arg_buffer[buffer_ptr - args_to_pass + i];
            }
            buffer_ptr -= args_to_pass;
            pc = target_address;
            continue;
        } 
        else if (strcmp(cmd.opcode, "BIND_PARAM") == 0) {
            write_var(cmd.arg1, call_stack[frame_ptr].passed_args[call_stack[frame_ptr].arg_index++]);
        } 
        else if (strcmp(cmd.opcode, "RETURN") == 0) {
            int return_value = read_var(cmd.arg1);
            int jump_back = call_stack[frame_ptr].return_pc;
            char dest_var[32];
            strcpy(dest_var, call_stack[frame_ptr].target_var);
            frame_ptr--; 
            write_var(dest_var, return_value);
            pc = jump_back;
            continue;
        }
        pc++;
    }

    printf("=== VM Execution Finished ===\n\nGlobal Variables:\n");
    for (int i = 0; i < call_stack[0].local_count; i++) {
        if (call_stack[0].var_names[i][0] != 't') { 
            printf(">> %s = %d\n", call_stack[0].var_names[i], call_stack[0].var_values[i]);
        }
    }
}