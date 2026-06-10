#include "tiny_compiler.h"

Token current_token;

void advance_token() {
    while (1) {
        while (isspace(*source_code)) source_code++;
        if (*source_code == '\0') {
            current_token.kind = TOK_EOF;
            return;
        }

        // Handle Comments
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