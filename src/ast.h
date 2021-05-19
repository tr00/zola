#ifndef __AST_H_
#define __AST_H_

enum {
    // flag = 0 => nil
    AST_FLAG_ATOM = 0x1,
    AST_FLAG_CONS = 0x2,
    AST_FLAG_LIST = 0x4,
    AST_FLAG_CALL = 0x8,
    AST_FLAG_BUILTIN = 0x10,
    AST_FLAG_LAMBDA = 0x20,
    AST_FLAG_MACRO = 0x40,
    AST_FLAG_NUMBER = 0x80,
    AST_FLAG_SYMBOL = 0x100,
};

/**
 * 
 */
struct SEXPR {
    unsigned flag; // atom / node // block / call / literal / symbol ...
    char* type; // sexpr :: type
    union {
        struct { char* atom; };
        struct {
            struct SEXPR* car; // NULL == nil
            struct SEXPR* cdr;
        };
    };
};

extern struct SEXPR nil;

#endif