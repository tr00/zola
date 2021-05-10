/**
 * Phase 1:
 *  - IR generation
 *  - type inference
 *  - compiler hints
 * 
 * Phase 2:
 *  - const propagation
 *  - const folding
 *  - subexpression elimination
 *  - dead store elimination
 */

#include <string.h>
#include <stdlib.h>

#include "ZL3.h"
#include "ZL2.h"
#include "ZL0.h"

static int hash(char* s)
{
    unsigned val;
    for(val = 0; *s != '\0'; s++)
    {
      val = *s + 31 * val;
    }
    return val % HASHSIZE;
}

struct ZL3_HASHMAP* lookup(struct ZL3_HASHMAP** map, char *s)
{
    struct ZL3_HASHMAP *np;
    for(np = map[hash(s)]; np != NULL; np = np->next)
    {
        if(strcmp(s, np->name) == 0)
        {
            return np;
        }
    }
    return NULL;
}

struct ZL3_HASHMAP* put(struct ZL3_HASHMAP** map, char* name, struct ZL3_IR_NODE* var)
{
    struct ZL3_HASHMAP* np;
    unsigned val;
    if((np = lookup(map, name)) == NULL)
    {
        np = ZL0_malloc(sizeof(struct ZL3_HASHMAP));
        val = hash(name);
        np->next = map[val];
        map[val] = np;
    }
    else
    {
        free(np->var);
    }
    np->var = var;
    return np;
}

static int isbuiltin(char* name)
{
    if(name == NULL) return 0;

    else if(0 == strcmp(name, "__add__")) return ZL3_BUILTIN_ADD;
    else if(0 == strcmp(name, "__sub__")) return ZL3_BUILTIN_SUB;
    else if(0 == strcmp(name, "__mul__")) return ZL3_BUILTIN_MUL;
    else if(0 == strcmp(name, "__udiv__")) return ZL3_BUILTIN_UDIV;
    else if(0 == strcmp(name, "__sdiv__")) return ZL3_BUILTIN_SDIV;
    else if(0 == strcmp(name, "__urem__")) return ZL3_BUILTIN_UREM;
    else if(0 == strcmp(name, "__srem__")) return ZL3_BUILTIN_SREM;
    else if(0 == strcmp(name, "__shl__")) return ZL3_BUILTIN_SHL;
    else if(0 == strcmp(name, "__lshr__")) return ZL3_BUILTIN_LSHR;
    else if(0 == strcmp(name, "__ashr__")) return ZL3_BUILTIN_ASHR;
    else if(0 == strcmp(name, "__and__")) return ZL3_BUILTIN_AND;
    else if(0 == strcmp(name, "__or__")) return ZL3_BUILTIN_OR;
    else if(0 == strcmp(name, "__xor__")) return ZL3_BUILTIN_XOR;

    return 0;
}

static struct ZL3_IR_NODE* variable_lookup(struct ZL3_IR_CONTEXT* ctx, char* name)
{
    struct ZL3_HASHMAP* np;
    if((np = lookup(ctx->variables, name)) != NULL)
    {
        return np->var;
    }
    else if(ctx->parent)
    {
        return variable_lookup(ctx->parent, name);
    }
    else
    {
        return NULL;
    }
}

struct ZL3_IR_NODE* ZL3_visit_atom(struct ZL2_AST_ATOM* atom, struct ZL3_IR_CONTEXT* ctx)
{
    /**
     * builtin ?
     * variable ?
     */
    ZL0_assert(atom, "ZL3_visit_atom( NULL )");

    struct ZL3_IR_NODE* node = ZL0_malloc(sizeof(struct ZL3_IR_NODE));

    if(atom->tag == ZL2_ATOM_INTEGER)
    {
        node->tag = ZL3_NODE_CONSTANT;
        node->val.constant.type = ZL3_TYPE_I32;
        node->val.constant.val = atom->val;

        free(atom);

        return node;
    }
    else if(atom->tag == ZL2_ATOM_SYMBOL)
    {

        /* builtin */
        char* name = atom->val;
        int cmp;
        if((cmp = isbuiltin(name)))
        {
            node->tag = ZL3_NODE_BUILTIN;
            node->val.builtin.tag = cmp;
            free(atom);
            return node;
        }
        struct ZL3_IR_NODE* var = variable_lookup(ctx, name);
        ZL0_assert(var, "undeclared variable");
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
    /**
     * TODO:
     *
     * each expr has to get marked whether it is statically compilable
     */

    if(expr->tag == ZL2_EXPR_ATOM)
    {

    }
    else if(expr->tag == ZL2_EXPR_LIST)
    {

    }
    else if(expr->tag == ZL2_EXPR_CALL)
    {

    }


    return NULL;
}

struct ZL3_IR_NODE* ZL3_visit_annotation()
{
    return NULL;
}

