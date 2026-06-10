// demo_lexer.c -- Standalone lexer for a tiny expression language (Chapter 3)
// Compile: gcc demo_lexer.c -o demo_lexer
// Run:     ./demo_lexer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    T_INT, T_ID, T_PLUS, T_MINUS, T_MUL, T_DIV,
    T_LPAREN, T_RPAREN, T_ASSIGN, T_SEMI, T_EOF
} TokenKind;

typedef struct { TokenKind kind; char text[32]; } Token;

const char *src;

Token next_token() {
    Token tok = { T_EOF, "" };
    while (*src && isspace(*src)) src++;
    if (!*src) return tok;

    if (isdigit(*src)) {
        int i = 0;
        while (isdigit(*src)) tok.text[i++] = *src++;
        tok.text[i] = '\0';
        tok.kind = T_INT;
    } else if (isalpha(*src)) {
        int i = 0;
        while (isalnum(*src)) tok.text[i++] = *src++;
        tok.text[i] = '\0';
        tok.kind = T_ID;
    } else {
        tok.text[0] = *src; tok.text[1] = '\0';
        switch (*src++) {
            case '+': tok.kind = T_PLUS;   break;
            case '-': tok.kind = T_MINUS;  break;
            case '*': tok.kind = T_MUL;    break;
            case '/': tok.kind = T_DIV;    break;
            case '(': tok.kind = T_LPAREN; break;
            case ')': tok.kind = T_RPAREN; break;
            case '=': tok.kind = T_ASSIGN; break;
            case ';': tok.kind = T_SEMI;   break;
        }
    }
    return tok;
}

const char *kind_name(TokenKind k) {
    switch (k) {
        case T_INT: return "INT";  case T_ID:    return "ID";
        case T_PLUS: return "+";   case T_MINUS: return "-";
        case T_MUL: return "*";    case T_DIV:   return "/";
        case T_LPAREN: return "("; case T_RPAREN: return ")";
        case T_ASSIGN: return "="; case T_SEMI:  return ";";
        case T_EOF: return "EOF";
    }
    return "?";
}

int main() {
    src = "x = (a + b) * 3;";
    printf("Source: %s\n\nTokens:\n", src);

    Token tok;
    do {
        tok = next_token();
        printf("  %-6s  '%s'\n", kind_name(tok.kind), tok.text);
    } while (tok.kind != T_EOF);

    return 0;
}
