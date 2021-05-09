/**
 * The Parser and its utilities
 */
#ifndef __ZL2_H_
#define __ZL2_H_

enum {
    ZL2_ATOM_SYMBOL = 20000,
    ZL2_ATOM_INTEGER = 20001,

    ZL2_NODE_EXPR = 21000,
    ZL2_NODE_ATOM = 21001,
};

typedef struct lexer_s lexer_t;

struct ZL2_AST_ATOM {
    int tag;
    char* val;
};

struct ZL2_AST_EXPR {
    struct ZL2_AST_NODE* head;
};

struct ZL2_AST_NODE {
    int tag;
    union {
        struct ZL2_AST_EXPR *expr;
        struct ZL2_AST_ATOM *atom;
    } val;
    struct ZL2_AST_NODE *next;
};


struct ZL2_AST_ATOM* ZL2_parse_atom(lexer_t* lex);
struct ZL2_AST_EXPR* ZL2_parse_expr(lexer_t* lex);

#endif
