/**
 *
 */
#ifndef __ZL3_H_
#define __ZL3_H_

#include "ZL2.h"

typedef struct __constant_s {
    int val;
    // type
} __constant_t;

typedef struct __variable_s {
    // type
    // name
} __variable_t;

typedef struct __function_s {
    // 
} __function_t;

typedef struct value_s value_t;
typedef struct context_s context_t;

typedef struct __builtin_s {
    value_t* (*f)(node_t*, context_t*);
    // rtype
    // name
} __builtin_t;

typedef struct value_s {
    enum {
        VT_CONSTANT,
        VT_VARIABLE,
        VT_FUNCTION,
        VT_BUILTIN,
        VT_NIL,
    } tag;
    union {
        __constant_t __constant;
        __variable_t __variable;
        __function_t __function;
        __builtin_t __builtin;
    } val;
} value_t;

typedef struct dict_s {
    struct dict_s *next;
    char* hash;
    __function_t* func;
} dict_t;

typedef struct context_s {
    struct context_s *parent;
    dict_t *ftable;
    // function table
    // variable table
} context_t;

value_t* ZL3_node(node_t* node, context_t* ctx);
value_t* ZL3_atom(atom_t* atom, context_t* ctx);
value_t* ZL3_expr(expr_t* expr, context_t* ctx);

#endif
