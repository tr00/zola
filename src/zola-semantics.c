#include <string.h>
#include <stdlib.h>

#include "ZL3.h"
#include "ZL2.h"
#include "ZL0.h"

struct ZL3_HASHMAP* lookup(struct ZL3_HASHMAP** map, char *s);

static int isbuiltin(char* name)
{
    if(name == NULL) return 0;

    else if(0 == strcmp(name, "__add__")) return ZL3_BUILTIN_ADD;
    else if(0 == strcmp(name, "__sub__")) return ZL3_BUILTIN_SUB;
    else if(0 == strcmp(name, "__mul__")) return ZL3_BUILTIN_MUL;
    else if(0 == strcmp(name, "__udiv__")) return ZL3_BUILTIN_UDIV;
    else if(0 == strcmp(name, "__div__")) return ZL3_BUILTIN_DIV;
    else if(0 == strcmp(name, "__urem__")) return ZL3_BUILTIN_UREM;
    else if(0 == strcmp(name, "__rem__")) return ZL3_BUILTIN_REM;
    else if(0 == strcmp(name, "__shl__")) return ZL3_BUILTIN_SHL;
    else if(0 == strcmp(name, "__shr__")) return ZL3_BUILTIN_SHR;
    else if(0 == strcmp(name, "__sar__")) return ZL3_BUILTIN_SAR;
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

/**
 * symbol: finds associated variable names
 * number: defaults the type to i64
 * 
 */
struct ZL3_IR_NODE* ZL3_visit_atom(struct ZL2_AST_ATOM* atom, struct ZL3_IR_CONTEXT* ctx)
{
    ZL0_assert(atom, "ZL3_visit_atom( NULL )");

    struct ZL3_IR_NODE* node = ZL0_malloc(sizeof(struct ZL3_IR_NODE));

    if(atom->tag == ZL2_ATOM_INTEGER)
    {
        node->tag = ZL3_NODE_CONSTANT;
        node->val.constant.type = ZL3_TYPE_I64;
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
        /* variable */
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

/**
 * visits each element individualy and links them together
 */
struct ZL3_IR_NODE* ZL3_visit_args(struct ZL2_AST_LIST* list, struct ZL3_IR_CONTEXT* ctx)
{
    struct ZL3_IR_NODE* node = ZL3_visit_expr(list->expr, ctx);
    list = list->next;

    while(list)
    {
        node->next = ZL3_visit_expr(list->expr, ctx);
        node = node->next;
        list = list->next;
    }

    return node;
}

/**
 * only call on head
 */
struct ZL3_IR_NODE* ZL3_visit_list(struct ZL2_AST_LIST* list, struct ZL3_IR_CONTEXT* ctx)
{
    ZL0_assert(list, "ZL3_visit_list( NULL )");
    ZL0_assert(list->expr, "ZL3_visit_list( corrupt )");
    struct ZL2_AST_LIST* head = list;

    // { expr; } === expr
    if(list->next == NULL)
    {
        return ZL3_visit_expr(list->expr, ctx);
    }
    struct ZL3_IR_NODE* node = ZL0_malloc(sizeof(struct ZL3_IR_NODE));
    struct ZL3_IR_CONTEXT* local = ZL0_malloc(sizeof(struct ZL3_IR_CONTEXT));
    local->parent = ctx;

    return node;
}

struct ZL3_IR_NODE* ZL3_visit_expr(struct ZL2_AST_EXPR* expr, struct ZL3_IR_CONTEXT* ctx)
{
    ZL0_assert(expr, "ZL3_visit_expr( NULL )");
    /**
     *
     * each expr has to get marked whether it is statically compilable
     */
    struct ZL3_IR_NODE* node = NULL;

    if(expr->tag == ZL2_EXPR_ATOM)
    {
        node = ZL3_visit_atom(expr->val.atom, ctx);
        if(expr->type)
        {
            ZL3_visit_annotation(expr->type, node, ctx);
        }
    }
    else if(expr->tag == ZL2_EXPR_LIST)
    {
        node = ZL3_visit_list(expr->val.list, ctx);
    }
    else if(expr->tag == ZL2_EXPR_CALL) /* f(a b c) */
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

    return node;
}

/**
 * operates inplace
 */
struct ZL3_IR_NODE* ZL3_visit_annotation(char* type, struct ZL3_IR_NODE* node, struct ZL3_IR_CONTEXT* ctx)
{

    return node;
}


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
