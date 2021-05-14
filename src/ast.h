#ifndef __AST_H_
#define __AST_H_

enum {
    AST_NODE_BLOCK,
    AST_NODE_CALL,
    AST_NODE_ATOM,
    AST_NODE_LAMBDA,
    AST_NODE_BUILTIN,

    AST_ATOM_VARIABLE,
    AST_ATOM_BUILTIN,
    AST_ATOM_LAMBDA,
    AST_ATOM_LITERAL,
};

/**
 * 
 */
struct AST_ATOM {
    int tag, flag;
    char* val;
};

/**
 * 
 */
struct AST_NODE {
    int tag;
    union {
        struct AST_ATOM* atom;
        struct AST_NODE* node;
    } val;
    char* type;
    struct AST_NODE* next;
};

#endif