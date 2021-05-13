/**
 * AST to IR compiler 
 */
#ifndef __ZL3_H_
#define __ZL3_H_

/* import */
struct ZL2_AST_LIST;
struct ZL2_AST_ATOM;
struct ZL2_AST_EXPR;

/* const */
enum {
    ZL3_BUILTIN_ADD = 30000,
    ZL3_BUILTIN_SUB = 30001,
    ZL3_BUILTIN_MUL = 30002,
    ZL3_BUILTIN_UDIV = 30003,
    ZL3_BUILTIN_DIV = 30004,
    ZL3_BUILTIN_UREM = 30005,
    ZL3_BUILTIN_REM = 30006,
    ZL3_BUILTIN_SHL = 30007,
    ZL3_BUILTIN_SHR = 30008,
    ZL3_BUILTIN_SAR = 30009,
    ZL3_BUILTIN_AND = 30010,
    ZL3_BUILTIN_OR = 30011,
    ZL3_BUILTIN_XOR = 30012,
    
    /* comparisons */

    ZL3_BUILTIN_CEQ = 30012,
    ZL3_BUILTIN_CNE = 30012,

    ZL3_BUILTIN_CSLE = 30012,
    ZL3_BUILTIN_CULE = 30012,
    ZL3_BUILTIN_CFLE = 30012,

    ZL3_BUILTIN_CSLT = 30012,
    ZL3_BUILTIN_CULT = 30012,
    ZL3_BUILTIN_CFLT = 30012,

    ZL3_BUILTIN_CSGE = 30012,
    ZL3_BUILTIN_CUGE = 30012,
    ZL3_BUILTIN_CFGE = 30012,

    ZL3_BUILTIN_CSGT = 30012,
    ZL3_BUILTIN_CUGT = 30012,
    ZL3_BUILTIN_CFGT = 30012,

    ZL3_BUILTIN_CFO = 30012,
    ZL3_BUILTIN_CFUO = 30012,

    ZL3_TYPE_I32 = 31000,
    ZL3_TYPE_I64 = 31001,
    ZL3_TYPE_U32 = 31002,
    ZL3_TYPE_U64 = 31003,
    ZL3_TYPE_F32 = 31004,
    ZL3_TYPE_F64 = 31005,

    ZL3_NODE_EXPR = 32000,
    ZL3_NODE_CONSTANT = 32001,
    ZL3_NODE_BUILTIN = 32002,
    ZL3_NODE_LAMBDA = 32002,
    ZL3_NODE_MACRO = 32002,

    ZL3_FLAG_INLINE = 0,
};

/* structs */
struct ZL3_IR_CONSTANT {
    int type;
    char* val;
};

struct ZL3_IR_BUILTIN {
    int tag;
};

struct ZL3_IR_EXPR {
    struct ZL3_IR_NODE* head;
};

struct ZL3_IR_LAMBDA {
    unsigned int flags;
    
};

struct ZL3_IR_NODE {
    int tag;
    union {
        struct ZL3_IR_BUILTIN builtin;
        struct ZL3_IR_LAMBDA lambda;
        struct ZL3_IR_CONSTANT constant;
        struct ZL3_IR_EXPR expr;
    } val;
    struct ZL3_IR_NODE* next;
};

struct ZL_CONTEXT {
    struct ZL_CONTEXT* parent;
};

/* functions */
void visit_type(char*, struct AST_NODE*, struct ZL_CONTEXT*);

#endif
