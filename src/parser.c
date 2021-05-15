
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "types.h"
#include "parser.h"
#include "scanner.h"
#include "frontend.h"

/**
 * TODO:
 *  - better error messages
 *  - implement macros
 *  - scan for memory leaks
 * 
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
 *      ()              == nil
 *      ( ex1 )         == ex1
 * 
 *      { }             == nil
 *      { ex1; }        == ex1
 *      { ex1; ex2; }   == cons(ex1 cons(ex2 nil) [block]) [block]
 * 
 *      f()             == cons(f nil) [call]
 *      { f; }()        == cons(f nil) [call]
 *      (g())()         == cons(cons(g nil) [call] nil) [call]
 */
;

struct SEXPR* parse_atom(lexer_t* lex)
{
    ZL0_assert(lex, "parse_atom( NULL )");
    struct TOKEN* tok = ZL1_lookahead(lex);
    if(tok->tag == ZL1_TOKEN_INTEGER)
    {
        struct SEXPR* sexpr = zlmalloc(sizeof(struct SEXPR));
        sexpr->atom = tok->val;
        sexpr->flag = AST_FLAG_ATOM | AST_FLAG_NUMBER;
        // atom->type == TYPE_I64
        free(ZL1_consume(lex));
        return sexpr;
    }
    else if(tok->tag == ZL1_TOKEN_SYMBOL)
    {
        struct SEXPR* sexpr = zlmalloc(sizeof(struct SEXPR));
        sexpr->atom = tok->val;
        sexpr->flag = AST_FLAG_ATOM | AST_FLAG_SYMBOL;
        free(ZL1_consume(lex));
        return sexpr;
    }
    return NULL;
}

struct SEXPR* parse_stmt(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_stmt( NULL )");
    struct SEXPR* expr = parse_expr(lex);

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

struct SEXPR* parse_list(lexer_t* lex)
{
    ZL0_assert(lex, "parse_list( NULL )");

#ifdef __TRACE_PARSER
    printf("ZL2_parse_list( ... ) start\n");
#endif

    struct TOKEN* tok = ZL1_lookahead(lex);

    if(tok->tag != ZL1_TOKEN_LBRACE)
        return NULL;
    
    free(ZL1_consume(lex));

    struct SEXPR* head = zlmalloc(sizeof(struct SEXPR));
    tok = ZL1_lookahead(lex);
    if(tok->tag == ZL1_TOKEN_RBRACE)
    {
        free(ZL1_consume(lex));
        head->flag = AST_FLAG_NIL;
        return head; // return nil
    }

    head->car = parse_stmt(lex);
    ZL0_assert(head->car, "expected statement or semicolon");

    tok = ZL1_lookahead(lex);
    struct SEXPR* tail = head;

    /* this section does one unnecessary malloc + free */
    while(tok->tag != ZL1_TOKEN_RBRACE)
    {
        ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

        struct SEXPR* node = ZL0_malloc(sizeof(struct SEXPR));

        node->car = parse_stmt(lex);
        ZL0_assert(node->car, "expected statement or closing brace");

        tail->cdr = node;
        tail = node;
        tok = ZL1_lookahead(lex);
    }

    free(ZL1_consume(lex));
    return head;
}


struct SEXPR* parse_expr(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_expr( NULL )");
    
    struct SEXPR* expr;

    if((expr = parse_list(lex)) != NULL)
    {
    }
    else if(ZL1_lookahead(lex)->tag == ZL1_TOKEN_LPAREN)
    {
        free(ZL1_consume(lex));
        expr = parse_expr(lex);
        ZL0_assert(ZL1_lookahead(lex)->tag == ZL1_TOKEN_RPAREN, "expected closing parenthesis");
        free(ZL1_consume(lex));
    }
    else
    {
        // atom
        expr = parse_atom(lex);
        
        if(expr == NULL)
            return NULL;
    }

    struct TOKEN* tok = ZL1_lookahead(lex); 
    if(tok->tag == ZL1_TOKEN_LPAREN)
    {
        free(ZL1_consume(lex));
        expr->type = NULL; // correct ?
        expr->flag |= AST_FLAG_CALL;

        tok = ZL1_lookahead(lex);
        if(tok->tag == ZL1_TOKEN_RPAREN)
        {
            free(ZL1_consume(lex));
            expr->cdr = zlmalloc(sizeof(struct SEXPR));
            expr->cdr->flag = AST_FLAG_NIL;
        }
        else
        {
            struct SEXPR* tail = zlmalloc(sizeof(struct SEXPR));
            expr->cdr = tail; // cons(f ...)
            tail->car = parse_expr(lex);
            ZL0_assert(tail->car, "expected expression or closing parenthesis");

            while((tok = ZL1_lookahead(lex))->tag != ZL1_TOKEN_RPAREN)
            {
                ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

                struct SEXPR* next = zlmalloc(sizeof(struct SEXPR));

                next->car = parse_expr(lex);
                ZL0_assert(next->car, "expected expression or closing parenthesis");

                tail->cdr = next;
                tail = next;
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
