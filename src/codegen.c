#define __TRACE_CODEGEN
#include <stdio.h>
#include <stdlib.h>

#include "frontend.h"
#include "ast.h"
#include "codegen.h"
#include "builtins.h"
#include "types.h"
#include "semantics.h"

typedef struct ref_s {
    unsigned char* name;
    unsigned char type;
} ref_t;

typedef struct block_s {
    unsigned int idx;
    FILE* out;
} block_t;

void codegen__add__(ref_t*, struct AST_NODE*, block_t*);
void codegen__mul__(ref_t*, struct AST_NODE*, block_t*);
void codegen__div__(ref_t*, struct AST_NODE*, block_t*);
void codegen__udiv__(ref_t*, struct AST_NODE*, block_t*);
void codegen__rem__(ref_t*, struct AST_NODE*, block_t*);
void codegen__urem__(ref_t*, struct AST_NODE*, block_t*);
void codegen__shl__(ref_t*, struct AST_NODE*, block_t*);
void codegen__shr__(ref_t*, struct AST_NODE*, block_t*);
void codegen__sar__(ref_t*, struct AST_NODE*, block_t*);
void codegen__and__(ref_t*, struct AST_NODE*, block_t*);
void codegen__xor__(ref_t*, struct AST_NODE*, block_t*);
void codegen__or__(ref_t*, struct AST_NODE*, block_t*);

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

void codegen_atom(ref_t* ref, struct AST_ATOM* node, block_t* block)
{

}

void codegen_node(ref_t* ref, struct AST_NODE* node, block_t* block)
{
    if(node->tag == AST_NODE_ATOM)
    {
        codegen_atom(ref, node->val.atom, block);
    }
    else if(node->tag == AST_NODE_BUILTIN)
    {
        struct AST_NODE* args = node->next;
        switch(node->val.atom->flag)
        {
            case __ADD__:
                codegen__add__(ref, args, block);
                break;
        } 
        // args have to get free'd by the builtin
        free(node);
    }
}

void codegen(struct AST_NODE* node)
{
    block_t* block = ZL0_malloc(sizeof(block_t));

    block->idx = 0;
    block->out = stdout;

    codegen_node(NULL, node, block);

    free(block);
}

void codegen__add__(ref_t* ref, struct AST_NODE* args, block_t* block)
{
    /* __add__(op1 op2) */

    ref_t *op1 = zlmalloc(sizeof(ref_t));
    ref_t *op2 = zlmalloc(sizeof(ref_t));

    codegen_node(op1, args, block);
    codegen_node(op2, args->next, block);

    if(op1->type != op2->type)
    {
        ZL0_fatal("type mismatch");
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