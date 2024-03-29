#ifndef __SCANNER_H_
#define __SCANNER_H_

enum {
    ZL1_TOKEN_EOF = 10000,
    ZL1_TOKEN_SYMBOL = 10001,
    ZL1_TOKEN_INTEGER = 10002,
    ZL1_TOKEN_LPAREN = 10003,
    ZL1_TOKEN_RPAREN = 10004,
    ZL1_TOKEN_LBRACE = 10005,
    ZL1_TOKEN_RBRACE = 10006,
    ZL1_TOKEN_COLONS = 10007,
    ZL1_TOKEN_SEMICOLON = 10008,
};

struct TOKEN {
    int tag;
    char* val;
};

typedef struct lexer_s lexer_t; // TODO: convert to new style

lexer_t* ZL1_create(char* src, char* filename);
struct TOKEN* __attribute__((always_inline)) lookahead(lexer_t* lex);
void consume(lexer_t* lex);
void ZL1_free(lexer_t* lex);
int predict(int token, lexer_t* lex);

#endif

