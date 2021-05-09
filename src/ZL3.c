#include <string.h>
#include <stdlib.h>

#include "ZL3.h"
#include "ZL2.h"
#include "ZL0.h"

struct ZL3_IR_NODE* ZL3_visit_atom(struct ZL2_AST_ATOM* atom)
{
    ZL0_assert(atom, "ZL3_visit_atom( NULL )");

    struct ZL3_IR_NODE* node = ZL0_malloc(sizeof(struct ZL3_IR_NODE));
    node->tag = ZL3_NODE_ATOM;

    if(atom->tag == ZL2_ATOM_INTEGER)
    {
        node->val.atom.tag = ZL3_ATOM_CONSTANT;
        node->val.atom.val.constant.type = ZL3_TYPE_I32;
        node->val.atom.val.constant.val = atom->val;

        free(atom);

        return node;
    }
    else if(atom->tag == ZL2_ATOM_SYMBOL)
    {

        /* builtin */
        char* name = atom->val;

        if(0 == strcmp(name, "add"))
        {
            node->val.atom.tag = ZL3_ATOM_BUILTIN;
            node->val.atom.val.builtin.tag = ZL3_BUILTIN_ADD;
        }
        else if(0 == strcmp(name, "sub"))
        {
            node->val.atom.tag = ZL3_ATOM_BUILTIN;
            node->val.atom.val.builtin.tag = ZL3_BUILTIN_SUB;
        }
        else if(0 == strcmp(name, "mul"))
        {
            node->val.atom.tag = ZL3_ATOM_BUILTIN;
            node->val.atom.val.builtin.tag = ZL3_BUILTIN_MUL;
        }
        else if(0 == strcmp(name, "udiv"))
        {
            node->val.atom.tag = ZL3_ATOM_BUILTIN;
            node->val.atom.val.builtin.tag = ZL3_BUILTIN_UDIV;
        }
        else if(0 == strcmp(name, "sdiv"))
        {
            node->val.atom.tag = ZL3_ATOM_BUILTIN;
            node->val.atom.val.builtin.tag = ZL3_BUILTIN_SDIV;
        }
        else if(0 == strcmp(name, "urem"))
        {
            node->val.atom.tag = ZL3_ATOM_BUILTIN;
            node->val.atom.val.builtin.tag = ZL3_BUILTIN_UREM;
        }
        else if(0 == strcmp(name, "srem"))
        {
            node->val.atom.tag = ZL3_ATOM_BUILTIN;
            node->val.atom.val.builtin.tag = ZL3_BUILTIN_SREM;
        }

        free(atom);
        return node;
    }
    else
    {
        ZL0_fatal("ZL3_visit_atom( corrupt )");
    }
}

struct ZL3_IR_NODE* ZL3_visit_expr(struct ZL2_AST_EXPR* expr)
{
    ZL0_assert(expr, "ZL3_visit_expr( NULL )");

    struct ZL3_IR_NODE* node = ZL0_malloc(sizeof(struct ZL3_IR_NODE));
    node->tag = ZL3_NODE_EXPR;

    if(expr->head)
    {
        struct ZL3_IR_NODE* head = node->val.expr.head = ZL3_visit_node(expr->head);
        
        if(head->tag == ZL3_NODE_ATOM)
        {
            if(head->val.atom.tag == ZL3_ATOM_BUILTIN)
            {
                ZL3_visit_builtin();
            }
        }
        // TODO:
    }
    return node;
}

struct ZL3_IR_NODE* ZL3_visit_node(struct ZL2_AST_NODE* node)
{
    ZL0_assert(node, "ZL3_visit_node( NULL )");
    if(node->tag == ZL2_NODE_ATOM)
    {
        return ZL3_visit_atom(node->val.atom);
    }
    else if(node->tag == ZL2_NODE_EXPR)
    {
        return ZL3_visit_expr(node->val.expr);
    }
    else
    {
        ZL0_fatal("ZL3_visit_node( corrupt )");
    }
}
