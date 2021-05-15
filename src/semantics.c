#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "semantics.h"
#include "ast.h"
#include "types.h"
#include "builtins.h"
#include "frontend.h"

/**
 * TODO:
 *  - complete builtin list
 *  - inline functions
 */

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

void analyze(struct SEXPR* sexpr)
{
    const unsigned flag = sexpr->flag;

    struct ZL_CONTEXT* ctx = NULL; // TODO: implement dispatcher
    if(flag & AST_FLAG_ATOM)
    {
        visit_atom(sexpr, ctx);
    }
    else if(flag & AST_FLAG_CALL)
    {
        visit_call(sexpr, ctx);
    }
    else if(flag & AST_FLAG_LIST)
    {
        visit_list(sexpr, ctx);
    }
}

void visit_atom(struct SEXPR* atom, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(atom, "visit_atom( NULL )");
    ZL0_assert(atom->flag & AST_FLAG_ATOM, "visit_atom( non-atom )");

    if(atom->flag & AST_FLAG_NUMBER)
    {
        // type = i64
    }
    else if(atom->flag & AST_FLAG_SYMBOL)
    {
        int cmp;
        if((cmp = isbuiltin(atom->atom)))
        {
            // atom->tag = AST_ATOM_BUILTIN;
            // atom->flag = cmp; // which builtin
        }
        else
        {
            // variable lookup
        }
    }

}

void visit_call(struct SEXPR* args, struct ZL_CONTEXT* ctx)
{
    // TODO: analyze head & maybe inline
    while(args)
    {
        visit_node(args->car, ctx);
        args = args->cdr;
    }
}

/**
 * only call on head
 */
void visit_list(struct SEXPR* block, struct ZL_CONTEXT* ctx)
{
    ZL0_assert(block, "visit_block( NULL )");
    ZL0_assert(block->car, "visit_block( corrupt )");
    struct AST_NODE* head = block;

    // { expr; } === expr
    if(block->cdr == NULL)
    {
        return visit_node(block->car, ctx);
    }
    printf("[WARNING]: only partial semantical analysis\n");
    // struct AST_NODE* node = ZL0_malloc(sizeof(struct AST_NODE));
    // struct ZL_CONTEXT* local = ZL0_malloc(sizeof(struct ZL_CONTEXT));
    // local->parent = ctx;
    // TODO: 
    // always inline this
}

void assert_type(char* type, struct SEXPR* node, struct ZL_CONTEXT* ctx)
{
    if(!strcmp(type, node->type))
    {
        // throw error 
    }
}