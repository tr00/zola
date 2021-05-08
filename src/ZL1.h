#ifndef __ZL1_H_
#define __ZL1_H_

typedef struct token_s {
    enum token_e {  
        TT_EOF = 37000,
        TT_SYMBOL = 37001,
        TT_INTEGER = 37002,
        TT_LPAREN = 37003,
        TT_RPAREN = 37004,
    } tag;
    char* val;
} token_t;

typedef struct lexer_s lexer_t;

lexer_t* ZL1_create(char* src, char* filename);
token_t* ZL1_lookahead(lexer_t* lex);
token_t* ZL1_consume(lexer_t* lex);
void ZL1_free(lexer_t* lex);

#endif

