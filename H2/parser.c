#include "tiny_compiler.h"

int temp_var_counter = 0;

void generate_temp(char *dest) { 
    sprintf(dest, "t%d", ++temp_var_counter); 
}

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
        
        if (current_token.kind == TOK_LPAREN) { 
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
        sprintf(program_ir[jump_f_loc].result, "%d", ir_index); 
        
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
        sprintf(program_ir[jump_f_loc].result, "%d", ir_index);          
        sprintf(program_ir[ir_index - 1].result, "%d", loop_start_pc);   

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