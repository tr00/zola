#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "semantics.h"
#include "ast.h"
#include "types.h"
#include "builtins.h"
#include "ZL0.h"

static int isbuiltin(char* name)
{
    if(name == NULL) return 0;

    else if(0 == strcmp(name, "__add__")) return __ADD__;
    else if(0 == strcmp(name, "__sub__")) return __SUB__;
    else if(0 == strcmp(name, "__mul__")) return __MUL__;
    else if(0 == strcmp(name, "__div__")) return __DIV__;
    else if(0 == strcmp(name, "__udiv__")) return __UDIV__;
    else if(0 == strcmp(name, "__rem__")) return __REM__;
    else if(0 == strcmp(name, "__urem__")) return __UREM__;
    else if(0 == strcmp(name, "__shl__")) return __SHL__;
    else if(0 == strcmp(name, "__shr__")) return __SHR__;
    else if(0 == strcmp(name, "__sar__")) return __SAR__;
    else if(0 == strcmp(name, "__and__")) return __AND__;
    else if(0 == strcmp(name, "__or__")) return __OR__;
    else if(0 == strcmp(name, "__xor__")) return __XOR__;

    return 0;
}

void visit_atom(struct AST_ATOM* atom, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(atom, "visit_atom( NULL )");
    if(atom->tag == AST_ATOM_LITERAL)
    {
        atom->flag = TYPE_I64;
    }
    else if(atom->tag == AST_ATOM_VARIABLE)
    {
        int cmp;
        if((cmp = isbuiltin(atom->val)))
        {
            atom->tag = AST_ATOM_BUILTIN;
            atom->flag = cmp; // which builtin
        }
        else
        {
            // variable lookup
        }
    }

}

/**
 * visits each element individualy and links them together
 */
void visit_args(struct AST_NODE* args, struct ZL_CONTEXT* ctx)
{
    while(args)
    {
        visit_node(args->val.node, ctx);
        args = args->next;
    }
}

/**
 * only call on head
 */
void visit_block(struct AST_NODE* block, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(block, "visit_block( NULL )");
    ZL0_assert(block->val.node, "visit_block( corrupt )");
    struct AST_NODE* head = block;

    // { expr; } === expr
    if(block->next == NULL)
    {
        return visit_node(block->val.node, ctx);
    }
    printf("[WARNING]: only partial semantical analysis");
    // struct AST_NODE* node = ZL0_malloc(sizeof(struct AST_NODE));
    // struct ZL_CONTEXT* local = ZL0_malloc(sizeof(struct ZL_CONTEXT));
    // local->parent = ctx;
    // TODO: 
    // always inline this
}

void visit_node(struct AST_NODE* node, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(node, "ZL3_visit_expr( NULL )");

    if(node->tag == AST_NODE_ATOM)
    {
        visit_atom(node->val.atom, ctx);
    }
    else if(node->tag == AST_NODE_BLOCK)
    {
        visit_block(node->val.node, ctx);
    }
    else if(node->tag == AST_NODE_CALL)
    {
        struct AST_NODE* head = node->val.node;
        visit_node(head, ctx);
        visit_args(node->next, ctx);
        if(head->tag == AST_NODE_LAMBDA)
        {
            // (lambda (x) { x; })( params ) => always inline
            if(1) // is actual lambda
            {
                // largs = head->val.next
                // lbody = head->val.next->next->val.node
                // beta-reduction
                // linked list insert
            }
            // (f() :: lambda_t)( params ) => cant inline
        }
        else if(head->tag == AST_NODE_ATOM)
        {
            // f(params) => maybe inline
        }
        else if(head->tag == AST_NODE_BUILTIN)
        {
            // __builtin__(params) => automatically inlines
        }
    }

    if(node->type)
    {
        visit_type(node->type, node, ctx);
    }

}

void visit_type(char* type, struct AST_NODE* node, struct ZL_CONTEXT* ctx)
{
    if(!strcmp(type, node->type))
    {
        // throw error 
    }
}