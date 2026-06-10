#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// 1. Intermediate Representation (IR) - Quadruples
// =========================================================
typedef struct {
    char opcode[16];   // e.g., ADD, JMP_F, CALL
    char arg1[32];     // First argument
    char arg2[32];     // Second argument
    char result[32];   // Destination or Jump Address
} IR_Instr;

IR_Instr program_ir[1000];
int ir_index = 0;

void emit_ir(const char *op, const char *a1, const char *a2, const char *res) {
    strcpy(program_ir[ir_index].opcode, op);
    strcpy(program_ir[ir_index].arg1, a1);
    strcpy(program_ir[ir_index].arg2, a2);
    strcpy(program_ir[ir_index].result, res);
    printf("%03d: %-10s %-10s %-10s %-10s\n", ir_index, op, a1, a2, res);
    ir_index++;
}

// =========================================================
// 2. Lexical Analyzer (Lexer)
// =========================================================
typedef enum {
    TOK_FUNC, TOK_RET, TOK_IF, TOK_WHILE, TOK_ID, TOK_NUM,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_COMMA, TOK_SEMI,
    TOK_ASSIGN, TOK_ADD, TOK_SUB, TOK_MUL, TOK_DIV, TOK_EQ, TOK_LT, TOK_GT, TOK_EOF
} TokenKind;

typedef struct {
    TokenKind kind;
    char lexeme[32];
} Token;

Token current_token;
const char *source_code;

void advance_token() {
    while (1) {
        while (isspace(*source_code)) source_code++;
        if (*source_code == '\0') {
            current_token.kind = TOK_EOF;
            return;
        }

        // Handle Comments: // or /* */
        if (*source_code == '/') {
            if (*(source_code + 1) == '/') {
                source_code += 2;
                while (*source_code != '\n' && *source_code != '\0') source_code++;
                continue;
            } else if (*(source_code + 1) == '*') {
                source_code += 2;
                while (!(*source_code == '*' && *(source_code + 1) == '/') && *source_code != '\0') source_code++;
                if (*source_code != '\0') source_code += 2;
                continue;
            }
        }
        break;
    }

    if (isdigit(*source_code)) {
        int i = 0;
        while (isdigit(*source_code)) current_token.lexeme[i++] = *source_code++;
        current_token.lexeme[i] = '\0';
        current_token.kind = TOK_NUM;
    } 
    else if (isalpha(*source_code) || *source_code == '_') {
        int i = 0;
        while (isalnum(*source_code) || *source_code == '_') current_token.lexeme[i++] = *source_code++;
        current_token.lexeme[i] = '\0';
        
        if (strcmp(current_token.lexeme, "func") == 0) current_token.kind = TOK_FUNC;
        else if (strcmp(current_token.lexeme, "return") == 0) current_token.kind = TOK_RET;
        else if (strcmp(current_token.lexeme, "if") == 0) current_token.kind = TOK_IF;
        else if (strcmp(current_token.lexeme, "while") == 0) current_token.kind = TOK_WHILE;
        else current_token.kind = TOK_ID;
    } 
    else {
        current_token.lexeme[0] = *source_code;
        current_token.lexeme[1] = '\0';
        switch (*source_code++) {
            case '(': current_token.kind = TOK_LPAREN; break;
            case ')': current_token.kind = TOK_RPAREN; break;
            case '{': current_token.kind = TOK_LBRACE; break;
            case '}': current_token.kind = TOK_RBRACE; break;
            case '+': current_token.kind = TOK_ADD; break;
            case '-': current_token.kind = TOK_SUB; break;
            case '*': current_token.kind = TOK_MUL; break;
            case '/': current_token.kind = TOK_DIV; break;
            case ',': current_token.kind = TOK_COMMA; break;
            case ';': current_token.kind = TOK_SEMI; break;
            case '<': current_token.kind = TOK_LT; break;
            case '>': current_token.kind = TOK_GT; break;
            case '=':
                if (*source_code == '=') {
                    source_code++;
                    current_token.kind = TOK_EQ;
                    strcpy(current_token.lexeme, "==");
                } else current_token.kind = TOK_ASSIGN;
                break;
        }
    }
}

// =========================================================
// 3. Parser (Recursive Descent)
// =========================================================
int temp_var_counter = 0;
void generate_temp(char *dest) { sprintf(dest, "t%d", ++temp_var_counter); }

void parse_expression(char *result);
void parse_statement();

void parse_factor(char *result) {
    if (current_token.kind == TOK_NUM) {
        generate_temp(result);
        emit_ir("LOAD_IMM", current_token.lexeme, "-", result);
        advance_token();
    } else if (current_token.kind == TOK_ID) {
        char identifier[32];
        strcpy(identifier, current_token.lexeme);
        advance_token();
        
        if (current_token.kind == TOK_LPAREN) { // Function Call
            advance_token();
            int arg_count = 0;
            while (current_token.kind != TOK_RPAREN) {
                char arg_res[32];
                parse_expression(arg_res);
                emit_ir("PUSH_ARG", arg_res, "-", "-");
                arg_count++;
                if (current_token.kind == TOK_COMMA) advance_token();
            }
            advance_token();
            generate_temp(result);
            char count_str[10];
            sprintf(count_str, "%d", arg_count);
            emit_ir("CALL", identifier, count_str, result);
        } else {
            strcpy(result, identifier);
        }
    } else if (current_token.kind == TOK_LPAREN) {
        advance_token();
        parse_expression(result);
        advance_token();
    }
}

void parse_term(char *result) {
    char left[32], right[32], temp[32];
    parse_factor(left);
    while (current_token.kind == TOK_MUL || current_token.kind == TOK_DIV) {
        char op[10];
        strcpy(op, current_token.kind == TOK_MUL ? "MUL" : "DIV");
        advance_token();
        parse_factor(right);
        generate_temp(temp);
        emit_ir(op, left, right, temp);
        strcpy(left, temp);
    }
    strcpy(result, left);
}

void parse_math_expr(char *result) {
    char left[32], right[32], temp[32];
    parse_term(left);
    while (current_token.kind == TOK_ADD || current_token.kind == TOK_SUB) {
        char op[10];
        strcpy(op, current_token.kind == TOK_ADD ? "ADD" : "SUB");
        advance_token();
        parse_term(right);
        generate_temp(temp);
        emit_ir(op, left, right, temp);
        strcpy(left, temp);
    }
    strcpy(result, left);
}

void parse_expression(char *result) {
    char left[32], right[32], temp[32];
    parse_math_expr(left);
    if (current_token.kind == TOK_EQ || current_token.kind == TOK_LT || current_token.kind == TOK_GT) {
        char op[10];
        if (current_token.kind == TOK_EQ) strcpy(op, "IS_EQ");
        else if (current_token.kind == TOK_LT) strcpy(op, "IS_LT");
        else strcpy(op, "IS_GT");
        
        advance_token();
        parse_math_expr(right);
        generate_temp(temp);
        emit_ir(op, left, right, temp);
        strcpy(result, temp);
    } else {
        strcpy(result, left);
    }
}

void parse_statement() {
    if (current_token.kind == TOK_IF) {
        advance_token(); advance_token();
        char condition[32];
        parse_expression(condition);
        advance_token(); advance_token();
        
        int jump_f_loc = ir_index;
        emit_ir("JMP_FALSE", condition, "-", "?"); 
        
        while (current_token.kind != TOK_RBRACE) parse_statement();
        advance_token();
        sprintf(program_ir[jump_f_loc].result, "%d", ir_index); // Backpatch
        
    } else if (current_token.kind == TOK_WHILE) {
        advance_token(); advance_token();
        int loop_start_pc = ir_index;
        char condition[32];
        parse_expression(condition);
        advance_token(); advance_token();
        
        int jump_f_loc = ir_index;
        emit_ir("JMP_FALSE", condition, "-", "?");
        
        while (current_token.kind != TOK_RBRACE) parse_statement();
        advance_token();
        
        emit_ir("JMP", "-", "-", "?");
        sprintf(program_ir[jump_f_loc].result, "%d", ir_index);          // Backpatch Exit
        sprintf(program_ir[ir_index - 1].result, "%d", loop_start_pc);   // Backpatch Loop Restart

    } else if (current_token.kind == TOK_ID) {
        char var_name[32];
        strcpy(var_name, current_token.lexeme);
        advance_token();
        if (current_token.kind == TOK_ASSIGN) {
            advance_token();
            char expr_res[32];
            parse_expression(expr_res);
            emit_ir("STORE", expr_res, "-", var_name);
            if (current_token.kind == TOK_SEMI) advance_token();
        }
    } else if (current_token.kind == TOK_RET) {
        advance_token();
        char expr_res[32];
        parse_expression(expr_res);
        emit_ir("RETURN", expr_res, "-", "-");
        if (current_token.kind == TOK_SEMI) advance_token();
    }
}

void compile_program() {
    while (current_token.kind != TOK_EOF) {
        if (current_token.kind == TOK_FUNC) {
            advance_token();
            char func_name[32];
            strcpy(func_name, current_token.lexeme);
            emit_ir("DEF_FUNC", func_name, "-", "-");
            advance_token(); advance_token();
            
            while (current_token.kind == TOK_ID) {
                emit_ir("BIND_PARAM", current_token.lexeme, "-", "-");
                advance_token();
                if (current_token.kind == TOK_COMMA) advance_token();
            }
            advance_token(); advance_token();
            while (current_token.kind != TOK_RBRACE) parse_statement();
            emit_ir("END_FUNC", func_name, "-", "-");
            advance_token();
        } else {
            parse_statement();
        }
    }
}

// =========================================================
// 4. Virtual Machine
// =========================================================
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
        if (call_stack[0].var_names[i][0] != 't') { // Hide temp variables
            printf(">> %s = %d\n", call_stack[0].var_names[i], call_stack[0].var_values[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) return 1;
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);

    source_code = buffer;
    printf("Intermediate Representation (IR):\n--------------------------------------------\n");

    advance_token();
    compile_program();
    execute_vm();

    free(buffer);
    return 0;
}