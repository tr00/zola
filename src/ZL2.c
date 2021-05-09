#include <stdlib.h>

#include "ZL2.h"
#include "ZL1.h"
#include "ZL0.h"

struct ZL2_AST_ATOM* ZL2_parse_atom(lexer_t* lex)
/* expr => clean */
/* NULL => leaks */
{
    if(lex)
    {
        /* alloc: 000 */
        /* maybe-alloc: 001 */
        struct ZL1_TOKEN* tok = ZL1_lookahead(lex);

        if(tok->tag == ZL1_TOKEN_INTEGER)
        {
            struct ZL2_AST_ATOM* atom = ZL0_malloc(sizeof(struct ZL2_AST_ATOM)); /* safe */
            atom->tag = ZL2_ATOM_INTEGER;
            atom->val = tok->val; /* recycle: 001 */

            free(ZL1_consume(lex)); /* free: 000 */
            return atom;
        }
        else if(tok->tag == ZL1_TOKEN_SYMBOL)
        {
            struct ZL2_AST_ATOM* atom = ZL0_malloc(sizeof(struct ZL2_AST_ATOM)); /* safe */
            atom->tag = ZL2_ATOM_SYMBOL;
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



struct ZL2_AST_EXPR* ZL2_parse_expr(lexer_t* lex)
/* expr => clean */
/* NULL => leaks */
{
    if(lex)
    {
        /* alloc: 002 */
        /* alloc: 003 */
        struct ZL1_TOKEN* tok = ZL1_lookahead(lex); 
    
        if(tok->tag == ZL1_TOKEN_LPAREN)
        {
            struct ZL2_AST_EXPR* expr = ZL0_malloc(sizeof(struct ZL2_AST_EXPR)); /* safe */
    
            /* free: 002 */
            /* null: 003 */
            free(ZL1_consume(lex));

            /* alloc: 004 */
            /* alloc: 005 */
            tok = ZL1_lookahead(lex); 

            if(tok->tag == ZL1_TOKEN_RPAREN)
            {
                /* free: 004 */
                /* null: 005 */
                free(ZL1_consume(lex));
                expr->head = NULL;
                return expr;
            }

            expr->head = ZL0_malloc(sizeof(struct ZL2_AST_NODE));
            expr->head->tag = ZL2_NODE_ATOM;
            
            if((expr->head->val.atom = ZL2_parse_atom(lex)) == NULL) /* else => free: 004 recycle: 005 */
            {
                expr->head->tag = ZL2_NODE_EXPR;
                if((expr->head->val.expr = ZL2_parse_expr(lex)) == NULL) /* else => leaking: 004 005 */
                {
                    /* dump */
                    ZL0_fatal("expected atom or expr");
                }
            }

            /* alloc: 006 */
            /* alloc: 007 */
            tok = ZL1_lookahead(lex);

            struct ZL2_AST_NODE* tail = expr->head; /* safe */
            while(tok->tag != ZL1_TOKEN_RPAREN)
            {
                if(tok->tag == ZL1_TOKEN_EOF)
                {
                    /* dump */
                    ZL0_fatal("unexpected end of input");
                }

                struct ZL2_AST_NODE* node = ZL0_malloc(sizeof(struct ZL2_AST_NODE)); /* safe */
                node->tag = ZL2_NODE_ATOM;
                /* free: 006|008 */
                /* null|recycle: 007|009 */
                if((node->val.atom = ZL2_parse_atom(lex)) == NULL) 
                {
                    node->tag = ZL2_NODE_EXPR;
                    if((node->val.expr = ZL2_parse_expr(lex)) == NULL)
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
