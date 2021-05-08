#include <stdlib.h>

#include "ZL2.h"
#include "ZL0.h"

atom_t* ZL2_atom(lexer_t* lex)
{
    if(lex)
    {
        token_t* tok = ZL1_lookahead(lex); /* safe */
    
        if(tok->tag == TT_INTEGER)
        {
            ZL1_consume(lex); /* safe */

            atom_t* atom = (atom_t*) ZL0_malloc(sizeof(atom_t)); /* safe */
            atom->tag = AT_INTEGER;
            atom->val = tok->val;

            free(tok); /* safe */
            return atom;
        }
        else if(tok->tag == TT_SYMBOL)
        {
            ZL1_consume(lex); /* safe */

            atom_t* atom = (atom_t*) ZL0_malloc(sizeof(atom_t)); /* safe */
            atom->tag = AT_SYMBOL;
            atom->val = tok->val;

            free(tok); /* safe */
            return atom;
        }
        return NULL;
    }

    ZL0_fatal("ZL2_atom( NULL )");
}


/**
 * expr ::= LPAREN [atom|expr]* RPAREN
 */
expr_t* ZL2_expr(lexer_t* lex)
{
    if(lex)
    {
        token_t* tok = ZL1_lookahead(lex); /* safe */
    
        if(tok->tag == TT_LPAREN)
        {
            expr_t* expr = ZL0_malloc(sizeof(expr_t)); /* safe */
    
            free(ZL1_consume(lex));
            tok = ZL1_lookahead(lex);

            // case: ()
            if(tok->tag == TT_RPAREN)
            {
                free(ZL1_consume(lex));
                expr->head = NULL;
                return expr;
            }

            // case: (x0)
            expr->head = ZL0_malloc(sizeof(node_t));
            if((expr->head->val.atom = ZL2_atom(lex)) == NULL)
            {
                if((expr->head->val.expr = ZL2_expr(lex)) == NULL)
                {
                    ZL0_fatal("expected atom or expr");
                }
            }
            tok = ZL1_lookahead(lex);

            // case: (x0 x1 ... xn)
            node_t* tail = expr->head; /* safe */
            while(tok->tag != TT_RPAREN)
            {
                if(tok->tag == TT_EOF)
                {
                    ZL0_fatal("unexpected end of input");
                }

                node_t* node = ZL0_malloc(sizeof(node_t)); /* safe */
            
                if((node->val.atom = ZL2_atom(lex)) == NULL)
                {
                    if((node->val.expr = ZL2_expr(lex)) == NULL)
                    {
                        ZL0_fatal("expected atom or expr");
                    }
                }

                tail->next = node;
                tail = node;

                tok = ZL1_lookahead(lex); /* safe */
            }

            free(ZL1_consume(lex));
            return expr;
        }
        return NULL;
    }

    ZL0_fatal("ZL2_expr( NULL )");
}
