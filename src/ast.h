#ifndef __AST_H_
#define __AST_H_

enum {
    AST_NODE_BLOCK,
    AST_NODE_CALL,
    AST_NODE_LITERAL,
    AST_NODE_SYMBOL,
};

struct AST_NODE;

/**
 * A linked list used for blocks, function arguments, etc.
 */
struct AST_LIST {
    struct AST_NODE* node;
    struct AST_LIST* next;
};

/**
 * 
 */
struct AST_SYMBOL {
    int tag;
    const char* val;
};

/**
 * 
 */
struct AST_LITERAL {
    const char* val;
    int type;
};

/**
 * 
 */
struct AST_NODE {
    int tag;
    union {
        struct AST_SYMBOL* symbol;
        struct AST_LITERAL* literal;
        struct AST_LIST* list; 
    } val;
    const char* type;
};

#endif