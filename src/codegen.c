#define __TRACE_CODEGEN
#include <stdio.h>
#include <stdlib.h>

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

void codegen__add__(ref_t*, struct SEXPR*, block_t*);
void codegen__mul__(ref_t*, struct SEXPR*, block_t*);
void codegen__div__(ref_t*, struct SEXPR*, block_t*);
void codegen__udiv__(ref_t*, struct SEXPR*, block_t*);
void codegen__rem__(ref_t*, struct SEXPR*, block_t*);
void codegen__urem__(ref_t*, struct SEXPR*, block_t*);
void codegen__shl__(ref_t*, struct SEXPR*, block_t*);
void codegen__shr__(ref_t*, struct SEXPR*, block_t*);
void codegen__sar__(ref_t*, struct SEXPR*, block_t*);
void codegen__and__(ref_t*, struct SEXPR*, block_t*);
void codegen__xor__(ref_t*, struct SEXPR*, block_t*);
void codegen__or__(ref_t*, struct SEXPR*, block_t*);

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

void codegen__add__(ref_t* ref, struct SEXPR* args, block_t* block)
{
    /* __add__(op1 op2) */

    ref_t *op1 = zlmalloc(sizeof(ref_t));
    ref_t *op2 = zlmalloc(sizeof(ref_t));

    codegen_node(op1, args, block);
    codegen_node(op2, args->cdr, block);

    if(op1->type != op2->type)
    {
        zlerror("type mismatch", NULL);
    }

    if(ref)
    {
        // ref->name = itoa(block->idx);
        ref->type = op1->type;
    }

    /*    %2 =l add %0, %1 */
    fprintf(block->out, "\t%%%d =%c add %s, %s\n", block->idx++, op1->type, op1->name, op2->name);

    free(op1->name);
    free(op2->name);
    free(op1);
    free(op2);
}