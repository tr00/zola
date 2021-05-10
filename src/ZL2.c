
#include <stdlib.h>
#include <stdio.h>

#include "ZL2.h"
#include "ZL1.h"
#include "ZL0.h"

/**
 * grammar rules:
 *
 *      atom ::= SYMBOL
 *      atom ::= INTEGER
 *      list ::= LBRACE stmt RBRACE
 *      expr ::= [list|atom|LPAREN expr RPAREN] [LPAREN expr* RPAREN] [COLONS SYMBOL]
 *      expr ::= LPAREN expr RPAREN
 *      stmt ::= expr SEMICOLON
 *
 *
 *      f()
 *      { f }()
 *      g()()
 */
;

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

struct ZL2_AST_EXPR* ZL2_parse_stmt(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_stmt( NULL )");
    struct ZL2_AST_EXPR* expr = ZL2_parse_expr(lex);

    if(expr == NULL)
    {
        return NULL;
    }
    else
    {
        ZL0_assert(ZL1_lookahead(lex)->tag == ZL1_TOKEN_SEMICOLON, "expected semicolon");
        free(ZL1_consume(lex));
        return expr;
    }
}

struct ZL2_AST_LIST* ZL2_parse_list(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_list( NULL )");

#ifdef __TRACE_PARSER
    printf("ZL2_parse_list( ... ) start\n");
#endif

    struct ZL1_TOKEN* tok = ZL1_lookahead(lex);

    if(tok->tag == ZL1_TOKEN_LBRACE)
    {
        free(ZL1_consume(lex));
        struct ZL2_AST_LIST* head = ZL0_malloc(sizeof(struct ZL2_AST_LIST));

        tok = ZL1_lookahead(lex);
        if(tok->tag == ZL1_TOKEN_RBRACE)
        {
            free(ZL1_consume(lex));
            head->expr = NULL;
            return head;
        }

        head->expr = ZL2_parse_stmt(lex);
        ZL0_assert(head->expr, "expected statement or semicolon");

        tok = ZL1_lookahead(lex);
        struct ZL2_AST_LIST* tail = head;

        /* this section does one unnecessary malloc + free */
        while(tok->tag != ZL1_TOKEN_RBRACE)
        {
            ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

            struct ZL2_AST_LIST* node = ZL0_malloc(sizeof(struct ZL2_AST_LIST));

            node->expr = ZL2_parse_stmt(lex);
            ZL0_assert(node->expr, "expected stamtement or closing brace");

            tail->next = node;
            tail = node;
            tok = ZL1_lookahead(lex);
        }

        free(ZL1_consume(lex));
        return head;
    }

    return NULL;
}


struct ZL2_AST_EXPR* ZL2_parse_expr(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_expr( NULL )");
    
    struct ZL2_AST_EXPR* expr = ZL0_malloc(sizeof(struct ZL2_AST_EXPR));

    if((expr->val.list = ZL2_parse_list(lex)) != NULL)
    {
        expr->tag = ZL2_EXPR_LIST;
    }
    else if((expr->val.atom = ZL2_parse_atom(lex)) != NULL)
    {
        expr->tag = ZL2_EXPR_ATOM;
    }
    else if(ZL1_lookahead(lex)->tag == ZL1_TOKEN_LPAREN)
    {
        free(ZL1_consume(lex));
        expr = ZL2_parse_expr(lex);
        ZL0_assert(ZL1_lookahead(lex)->tag == ZL1_TOKEN_RPAREN, "expected closing parenthesis");
        free(ZL1_consume(lex));
    }
    else
    {
        return NULL;
    }

    struct ZL1_TOKEN* tok = ZL1_lookahead(lex); 
    if(tok->tag == ZL1_TOKEN_LPAREN)
    {
        free(ZL1_consume(lex));
        struct ZL2_AST_LIST* call = ZL0_malloc(sizeof(struct ZL2_AST_LIST));
        call->expr = expr;
        expr = ZL0_malloc(sizeof(struct ZL2_AST_EXPR));
        expr->val.list = call;
        expr->type = NULL;
        expr->tag = ZL2_EXPR_CALL;

        tok = ZL1_lookahead(lex);
        if(tok->tag == ZL1_TOKEN_RPAREN)
        {
            free(ZL1_consume(lex));
            call->next = NULL;
        }
        else
        {
            struct ZL2_AST_LIST* head = ZL0_malloc(sizeof(struct ZL2_AST_LIST));
            call->next = head;
            head->expr = ZL2_parse_expr(lex);
            ZL0_assert(head->expr, "expected expression or closing parenthesis");

            struct ZL2_AST_LIST* tail = head;
            while((tok = ZL1_lookahead(lex))->tag != ZL1_TOKEN_RPAREN)
            {
                ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

                struct ZL2_AST_LIST* node = ZL0_malloc(sizeof(struct ZL2_AST_LIST));

                node->expr = ZL2_parse_expr(lex);
                ZL0_assert(node->expr, "expected expression or closing parenthesis");

                tail->next = node;
                tail = node;
            }
            free(ZL1_consume(lex));
        }
    }
    
    if(tok->tag == ZL1_TOKEN_COLONS)
    {
        free(ZL1_consume(lex));
        ZL0_assert(ZL1_lookahead(lex)->tag == ZL1_TOKEN_SYMBOL, "expected type symbol");
        expr->type = ZL1_lookahead(lex)->val;
        free(ZL1_consume(lex));
        
    }

    return expr;
}

void ZL2_print_atom(struct ZL2_AST_ATOM atom)
{
    printf("atom[%d|%s]", atom.tag, atom.val ? atom.val : "NULL");
}

void ZL2_print_list(struct ZL2_AST_LIST list)
{
    printf("list[");
    while(list.expr)
    {
        printf("\n");
        ZL2_print_expr(*list.expr);
        if(list.next)
        {
            list = *list.next;
        }
        else
        {
            break;
        }
    }
    printf("]\n");
}

void ZL2_print_expr(struct ZL2_AST_EXPR expr)
{
    if(expr.tag == ZL2_EXPR_CALL)
    {
        printf("call[");
        ZL2_print_expr(*expr.val.list->expr);
        printf("|");
        if(expr.val.list->next)
        {
            ZL2_print_list(*expr.val.list->next);
        }
        printf("]");
    }
    else if(expr.tag == ZL2_EXPR_ATOM)
    {
        ZL2_print_atom(*expr.val.atom);
    }
    else if(expr.tag == ZL2_EXPR_LIST)
    {
        ZL2_print_list(*expr.val.list);
    }
}




