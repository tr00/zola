#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ZL0.h"
#include "ZL2.h"
#include "ZL3.h"

value_t* __builtin_add(node_t* node, context_t* ctx)
{
    value_t *sum, *tmp;
    
    sum = ZL3_node(node, ctx);

    if(sum->tag != VT_CONSTANT)
    {
        ZL0_fatal("mismatching type");
    }

    node = node->next;
    if(node == NULL)
    {
        ZL0_fatal("too few arguments");
    }

    while(node)
    {
        tmp = ZL3_node(node, ctx);

        if(tmp->tag != VT_CONSTANT)
        {
            ZL0_fatal("mismatching type");
        }
        else
        {
            sum->val.__constant.val += tmp->val.__constant.val;
        }
        node = node->next;
    }
    return sum;
}

value_t* ZL3_node(node_t* node, context_t* ctx)
{
    if(node)
    {
        if(node->tag == NT_ATOM)
        {
            return ZL3_atom(node->val.atom, ctx);
        }
        else if(node->tag == NT_EXPR)
        {
            return ZL3_expr(node->val.expr, ctx);
        }
        ZL0_fatal("ZL3_node( corrupt, ... )");
    }

    ZL0_fatal("ZL3_node( NULL, ... )");
}

value_t* ZL3_atom(atom_t* atom, context_t* ctx)
{
    value_t* value = (value_t*) ZL0_malloc(sizeof(value_t));
    if(atom->tag == AT_INTEGER)
    {
        value->val.__constant.val = strtol(atom->val, NULL, 10);
        // set type
        value->tag = VT_CONSTANT;
    }
    else
    {
        char* fname = atom->val;
        // builtin lookup
        if(0 == strcmp(fname, "add"))
        {
            value->val.__builtin.f = &__builtin_add;
            value->tag = VT_BUILTIN;
        }
        // function lookup
        // variable lookup
        // error
        else
        {
            ZL0_fatal("undefined variable");
        }
    }
    return value;
}

value_t* ZL3_expr(expr_t* expr, context_t* ctx)
{
    value_t* value = (value_t*) ZL0_malloc(sizeof(value_t));

    if(expr->head)
    {
        value_t* head = ZL3_node(expr->head, ctx);

        if(head->tag == VT_BUILTIN)
        {
            value = head->val.__builtin.f(expr->head->next, ctx);
        }
    }
    else
    {
        value->tag = VT_NIL;
    }
    return value;
}

