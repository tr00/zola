#include <string.h>
#include <stdlib.h>

#include "ast.h"
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

void visit_literal(struct AST_LITERAL* _, struct ZL_CONTEXT* ctx)
{
    // noop
}

void visit_symbol(struct AST_SYMBOL* symbol, struct ZL_CONTEXT* ctx)
{
    int cmp;
    if(cmp = isbuiltin(symbol->val))
    {
        symbol->tag = AST_SYMBOL_BUILTIN;
        symbol->flag = cmp;
    }
    // TODO: variable lookup
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
void visit_list(struct AST_LIST* list, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(list, "ZL3_visit_list( NULL )");
    ZL0_assert(list->node, "ZL3_visit_list( corrupt )");
    struct ZL2_AST_LIST* head = list;

    // { expr; } === expr
    if(list->next == NULL)
    {
        return ZL3_visit_expr(list->node, ctx);
    }
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

    if(node->tag == AST_NODE_LITERAL)
    {
        visit_literal(node->val.literal, ctx);
    }
    else if(node->tag == AST_NODE_SYMBOL)
    {
        visit_symbol(node->val.symbol, ctx);
    }
    else if(node->tag == AST_NODE_BLOCK)
    {
        ZL3_visit_list(node->val.list, ctx);
    }
    else if(node->tag == AST_NODE_CALL) /* f(a b c) */
    {
        node = ZL3_visit_expr(expr->val.list->expr, ctx);
        if(node->tag == ZL3_NODE_LAMBDA)
        {
        }
        else if(node->tag == ZL3_NODE_BUILTIN)
        {
            /* __builtin__(expr[a b c]) */
            node->next = ZL3_visit_args(expr->val.list->next, ctx);
        }
    }

    if(node->type)
    {
        ZL3_visit_annotation(expr->type, node, ctx);
    }

    return node;
}

void visit_type(char* type, struct AST_NODE* node, struct ZL_CONTEXT* ctx)
{

    return node;
}