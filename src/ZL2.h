/**
 * The Parser and its utilities
 */
#ifndef __ZL2_H_
#define __ZL2_H_

#include "ZL1.h"

typedef struct node_s node_t;

typedef struct atom_s {
    enum { 
        AT_SYMBOL = 38000,
        AT_INTEGER = 38001,
    } tag;
    char* val;
} atom_t;

typedef struct expr_s {
    node_t* head;
} expr_t;

typedef struct node_s {
    enum { 
        NT_ATOM = 39000, 
        NT_EXPR = 39001,
    } tag;
    union expr_u {
        expr_t *expr;
        atom_t *atom;
    } val;
    struct node_s *next;
} node_t;

atom_t* ZL2_atom(lexer_t* lex);
expr_t* ZL2_expr(lexer_t* lex);

#endif
