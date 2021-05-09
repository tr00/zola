#ifndef __ZL1_H_
#define __ZL1_H_

enum {
    ZL1_TOKEN_EOF = 10000,
    ZL1_TOKEN_SYMBOL = 10001,
    ZL1_TOKEN_INTEGER = 10002,
    ZL1_TOKEN_LPAREN = 10003,
    ZL1_TOKEN_RPAREN = 10004,
};

struct ZL1_TOKEN {
    int tag;
    char* val;
};

typedef struct lexer_s lexer_t; // TODO: convert to new style

lexer_t* ZL1_create(char* src, char* filename);
struct ZL1_TOKEN* ZL1_lookahead(lexer_t* lex);
struct ZL1_TOKEN* ZL1_consume(lexer_t* lex);
void ZL1_free(lexer_t* lex);

#endif

