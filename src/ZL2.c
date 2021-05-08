#include <stdlib.h>

#include "ZL2.h"
#include "ZL1.h"
#include "ZL0.h"

atom_t* ZL2_atom(lexer_t* lex)
/* expr => clean */
/* NULL => leaks */
{
    if(lex)
    {
        /* alloc: 000 */
        /* maybe-alloc: 001 */
        token_t* tok = ZL1_lookahead(lex);

        if(tok->tag == TT_INTEGER)
        {
            atom_t* atom = (atom_t*) ZL0_malloc(sizeof(atom_t)); /* safe */
            atom->tag = AT_INTEGER;
            atom->val = tok->val; /* recycle: 001 */

            free(ZL1_consume(lex)); /* free: 000 */
            return atom;
        }
        else if(tok->tag == TT_SYMBOL)
        {
            atom_t* atom = (atom_t*) ZL0_malloc(sizeof(atom_t)); /* safe */
            atom->tag = AT_SYMBOL;
            atom->val = tok->val; /* recycle: 001 */

            free(ZL1_consume(lex)); /* free: 000 */
            return atom;
        }

        /* leaking: 000 */
        /* null: 001 */
        return NULL;
    }

    ZL0_fatal("ZL2_atom( NULL )");
}



expr_t* ZL2_expr(lexer_t* lex)
/* expr => clean */
/* NULL => leaks */
{
    if(lex)
    {
        /* alloc: 002 */
        /* alloc: 003 */
        token_t* tok = ZL1_lookahead(lex); 
    
        if(tok->tag == TT_LPAREN)
        {
            expr_t* expr = ZL0_malloc(sizeof(expr_t)); /* safe */
    
            /* free: 002 */
            /* null: 003 */
            free(ZL1_consume(lex));

            /* alloc: 004 */
            /* alloc: 005 */
            tok = ZL1_lookahead(lex); 

            if(tok->tag == TT_RPAREN)
            {
                /* free: 004 */
                /* null: 005 */
                free(ZL1_consume(lex));
                expr->head = NULL;
                return expr;
            }

            expr->head = ZL0_malloc(sizeof(node_t));
            expr->head->tag = NT_ATOM;
            
            if((expr->head->val.atom = ZL2_atom(lex)) == NULL) /* else => free: 004 recycle: 005 */
            {
                expr->head->tag = NT_EXPR;
                if((expr->head->val.expr = ZL2_expr(lex)) == NULL) /* else => leaking: 004 005 */
                {
                    /* dump */
                    ZL0_fatal("expected atom or expr");
                }
            }

            /* alloc: 006 */
            /* alloc: 007 */
            tok = ZL1_lookahead(lex);

            node_t* tail = expr->head; /* safe */
            while(tok->tag != TT_RPAREN)
            {
                if(tok->tag == TT_EOF)
                {
                    /* dump */
                    ZL0_fatal("unexpected end of input");
                }

                node_t* node = ZL0_malloc(sizeof(node_t)); /* safe */
                node->tag = NT_ATOM;
                /* free: 006|008 */
                /* null|recycle: 007|009 */
                if((node->val.atom = ZL2_atom(lex)) == NULL) 
                {
                    node->tag = NT_EXPR;
                    if((node->val.expr = ZL2_expr(lex)) == NULL)
                    {
                        /* dump */
                        ZL0_fatal("expected atom or expr");
                    }
                }

                tail->next = node;
                tail = node;

                /* alloc: 008 */
                /* alloc: 009 */
                tok = ZL1_lookahead(lex); /* safe */
            }

            /* free: 008 */
            /* null: 009 */
            free(ZL1_consume(lex));
            return expr;
        }
        
        /* leaking: 002 */
        /* leaking: 003 */
        return NULL;
    }

    ZL0_fatal("ZL2_expr( NULL )");
}
