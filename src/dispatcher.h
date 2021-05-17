#ifndef __DISPATCHER_H_
#define __DISPATCHER_H_

struct SEXPR;

enum {
    VAR_FLAG_CALLABLE = 0x1,
    VAR_FLAG_INLINE = 0x2,
    VAR_FLAG_INITIALIZED = 0x4,
    VAR_FLAG_MUTATED = 0x8,
    VAR_FLAG_RECURSIVE = 0x10,
    VAR_FLAG_CONSTANT = 0x20,
    VAR_FLAG_PURE = 0x40,
};

struct VARIABLE {
    unsigned flag;
    const char* name;
    const char* type; // lambda_t in case of a function
};

struct CALLABLE {
    int unique;
    const char* rtype;
    const char** argt;
} CALLABLE;

void dispatch();

#endif