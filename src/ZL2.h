/**
 * The Parser and its utilities
 */
#ifndef __ZL2_H_
#define __ZL2_H_

enum {
    ZL2_ATOM_SYMBOL = 20000,
    ZL2_ATOM_INTEGER = 20001,

    ZL2_EXPR_ATOM = 21000,
    ZL2_EXPR_CALL = 21001,
    ZL2_EXPR_LIST = 21002,
};

typedef struct lexer_s lexer_t;

struct ZL2_AST_ATOM {
    int tag;
    char* val;
};

struct ZL2_AST_LIST {
    struct ZL2_AST_EXPR* expr;
    struct ZL2_AST_LIST* next;
};

struct ZL2_AST_EXPR {
    int tag;
    union {
        struct ZL2_AST_LIST* list;
        struct ZL2_AST_ATOM* atom;
    } val;
    char* type;
};

struct ZL2_AST_ATOM* ZL2_parse_atom(lexer_t* lex);
struct ZL2_AST_LIST* ZL2_parse_list(lexer_t* lex);
struct ZL2_AST_EXPR* ZL2_parse_expr(lexer_t* lex);
struct ZL2_AST_EXPR* ZL2_parse_stmt(lexer_t* lex);

void ZL2_print_atom(struct ZL2_AST_ATOM atom);
void ZL2_print_list(struct ZL2_AST_LIST list);
void ZL2_print_expr(struct ZL2_AST_EXPR expr);
    
#endif
