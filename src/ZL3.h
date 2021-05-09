/**
 * AST to IR compiler 
 */
#ifndef __ZL3_H_
#define __ZL3_H_

/* import */
struct ZL2_AST_NODE;
struct ZL2_AST_ATOM;
struct ZL2_AST_EXPR;

/* const */
enum {
    ZL3_BUILTIN_ADD = 30000,
    ZL3_BUILTIN_SUB = 30001,
    ZL3_BUILTIN_MUL = 30002,
    ZL3_BUILTIN_UDIV = 30003,
    ZL3_BUILTIN_SDIV = 30004,
    ZL3_BUILTIN_UREM = 30005,
    ZL3_BUILTIN_SREM = 30006,
    ZL3_BUILTIN_SHL = 30007,
    ZL3_BUILTIN_LSHR = 30008,
    ZL3_BUILTIN_ASHR = 30009,
    ZL3_BUILTIN_AND = 30010,
    ZL3_BUILTIN_OR = 30011,
    ZL3_BUILTIN_XOR = 30012,

    ZL3_TYPE_I32 = 31000,

    ZL3_NODE_EXPR = 32000,
    ZL3_NODE_ATOM = 32001,

    ZL3_ATOM_CONSTANT = 33000,
    ZL3_ATOM_BUILTIN = 33001,
};

/* structs */
struct ZL3_IR_CONST {
    int type;
    char* val;
};

struct ZL3_IR_BUILTIN {
    int tag;
};

struct ZL3_IR_ATOM {
    int tag;
    union {
        struct ZL3_IR_CONST constant;
        struct ZL3_IR_BUILTIN builtin;
    } val;
};

struct ZL3_IR_EXPR {
    struct ZL3_IR_NODE* head;
};

struct ZL3_IR_NODE {
    int tag;
    union {
        struct ZL3_IR_ATOM atom;
        struct ZL3_IR_EXPR expr;
    } val;
    struct ZL3_IR_NODE* next;
};

/* functions */
struct ZL3_IR_NODE* ZL3_visit_node(struct ZL2_AST_NODE* node);
struct ZL3_IR_NODE* ZL3_visit_atom(struct ZL2_AST_ATOM* atom);
struct ZL3_IR_NODE* ZL3_visit_expr(struct ZL2_AST_EXPR* expr);
struct ZL3_IR_NODE* ZL3_visit_builtin(struct ZL2_AST_EXPR* expr);

#endif
