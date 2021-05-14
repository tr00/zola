#include <string.h>
#include <stdlib.h>

#include "ast.h"
#include "types.h"
#include "builtins.h"
#include "ZL3.h"
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
        if(cmp = isbuiltin(atom->val))
        {
            atom->tag == AST_ATOM_BUILTIN;
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
void ZL3_visit_args(struct AST_LIST* list, struct ZL_CONTEXT* ctx)
{
    struct AST_NODE* node = ZL3_visit_expr(list->node, ctx);
    list = list->next;

    while(list)
    {
        node->next = ZL3_visit_expr(list->node, ctx);
        node = node->next;
        list = list->next;
    }
}

/**
 * only call on head
 */
void visit_block(struct AST_NODE* block, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(block, "visit_block( NULL )");
    ZL0_assert(block->val.node, "visit_block( corrupt )");
    struct ZL2_AST_LIST* head = block;

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
}

void visit_node(struct AST_NODE* node, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(node, "ZL3_visit_expr( NULL )");
    /**
     *
     * each expr has to get marked whether it is statically compilable
     */
    struct ZL3_IR_NODE* node = NULL;

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
        if(node->tag == AST_NODE_LAMBDA)
        {
            // (lambda (x) { x; })( params )
            // (f() :: lambda_t)( params )
        }
        else if(node->tag == AST_NODE_ATOM)
        {
            // f(params)
        }
        else if(node->tag == AST_NODE_BUILTIN)
        {
            // __builtin__(params)
        }
    }

    if(node->type)
    {
        ZL3_visit_type(node->type, node, ctx);
    }

    return node;
}

void visit_type(char* type, struct AST_NODE* node, struct ZL_CONTEXT* ctx)
{

    return node;
}