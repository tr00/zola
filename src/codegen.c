#define __TRACE_CODEGEN
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "frontend.h"
#include "ast.h"
#include "codegen.h"
#include "builtins.h"
#include "types.h"
#include "semantics.h"
#include "errors.h"

typedef struct ref_s {
    unsigned char* name;
    unsigned char type;
} ref_t;

typedef struct block_s {
    unsigned int idx;
    FILE* out;
} block_t;

static void __attribute__((always_inline)) newtmp(char* buffer, block_t* block) {
    sprintf(buffer, "%%%d", block->idx++);
}

void codegen__addl__(char*, struct SEXPR*, block_t*);

static char iltype(const int type)
{
    switch(type)
    {
        case TYPE_U32:
        case TYPE_I32:
            return 'w';
        case TYPE_U64:
        case TYPE_I64:
            return 'l';
        case TYPE_F32:
            return 's';
        case TYPE_F64:
            return 'd';
    }
    return '\0';
}

void codegen_atom(ref_t* ref, struct SEXPR* node, block_t* block)
{

}

void codegen_node(ref_t* ref, struct SEXPR* node, block_t* block)
{
    if(node->flag & AST_FLAG_ATOM)
    {
        codegen_atom(ref, node, block);
    }
    else if(node->flag & AST_FLAG_CALL)
    {
        struct SEXPR* args = node->cdr;
        if(node->flag & AST_FLAG_BUILTIN)
        {
        }
        else if(node->flag & AST_FLAG_SYMBOL)
        {

        }
        else
        {
            zlfatal("i dunno what to do here");
        }
    }
    free(node);
}

void codegen(struct SEXPR* node)
{
    block_t* block = zlmalloc(sizeof(block_t));

    block->idx = 0;
    block->out = stdout;

    codegen_node(NULL, node, block);

    free(block); // ?!
}

/**
 * the following functions all assume:
 *      - the parameter types match
 *      - there are only two parameters
 */

#define BINOP(instr, type) \
    void codegen__ ## instr ## type ## __(char* buf, struct SEXPR* args, block_t* block) \
    { \
        char *op1, *op2; \
        codegen_node(op1, args, block); \
        codegen_node(op2, args->cdr, block); \
        newtmp(buf, block); \ 
        fprintf(block->out, "\t%s =%s %s %s, %s\n", buf, #type, #instr, op1, op2); \
    }

BINOP(add, l)
BINOP(add, w)

BINOP(sub, l)
BINOP(sub, w)