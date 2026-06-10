#ifndef TINY_COMPILER_H
#define TINY_COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==========================================
// Lexer Definitions
// ==========================================
typedef enum {
    TOK_FUNC, TOK_RET, TOK_IF, TOK_WHILE, TOK_ID, TOK_NUM,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_COMMA, TOK_SEMI,
    TOK_ASSIGN, TOK_ADD, TOK_SUB, TOK_MUL, TOK_DIV, TOK_EQ, TOK_LT, TOK_GT, TOK_EOF
} TokenKind;

typedef struct {
    TokenKind kind;
    char lexeme[32];
} Token;

// Shared Global Variables for Lexer
extern Token current_token;
extern const char *source_code;

// Lexer Function Prototypes
void advance_token();

// ==========================================
// Code Generation (IR) Definitions
// ==========================================
typedef struct {
    char opcode[16];   // e.g., ADD, JMP_F, CALL
    char arg1[32];     // First argument
    char arg2[32];     // Second argument
    char result[32];   // Destination or Jump Address
} IR_Instr;

// Shared Global Variables for CodeGen
extern IR_Instr program_ir[1000];
extern int ir_index;

// CodeGen Function Prototypes
void emit_ir(const char *op, const char *a1, const char *a2, const char *res);

// ==========================================
// Parser Definitions
// ==========================================
void compile_program();

// ==========================================
// Virtual Machine Definitions
// ==========================================
void execute_vm();

#endif // TINY_COMPILER_H