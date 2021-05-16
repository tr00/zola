
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "types.h"
#include "parser.h"
#include "scanner.h"
#include "frontend.h"
#include "errors.h"

const struct SEXPR nil = { .flag = 0 };

/**
 * TODO:
 *  - better error messages
 *  - implement macros
 *  - scan for memory leaks
 *  - clarify ambiguous grammar rules
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

int parse_atom(struct SEXPR* atom, lexer_t* lex)
{
    zlassert(lex, "parse_atom( NULL )");
    struct TOKEN* tok = lookahead(lex);
    if(tok->tag == ZL1_TOKEN_INTEGER)
    {
        atom->atom = tok->val;
        atom->flag = AST_FLAG_ATOM | AST_FLAG_NUMBER;
        atom->type = "i64";
        consume(lex);
        return success;
    }
    else if(tok->tag == ZL1_TOKEN_SYMBOL)
    {
        atom->atom = tok->val;
        atom->flag = AST_FLAG_ATOM | AST_FLAG_SYMBOL;
        consume(lex);
        return success;
    }
    return failure;
}

int parse_stmt(struct SEXPR* stmt, lexer_t* lex)
{
    zlassert(lex, "parse_stmt( NULL )");
    
    if(parse_expr(stmt, lex))
        return failure;

    if(predict(ZL1_TOKEN_SEMICOLON, lex))
        zlerror("expected semicolon", NULL);

    return success;
}

int parse_list(struct SEXPR* list, lexer_t* lex)
{
    zlassert(lex, "parse_list( NULL )");

    if(predict(ZL1_TOKEN_LBRACE, lex))
        return failure;

    if(predict(ZL1_TOKEN_RBRACE, lex) == success)
    {
        *list = nil;
        return success;
    }

    if(parse_stmt(list->car, lex))
        zlerror("expected statement or semicolon", NULL);

    struct SEXPR* tail = list;

    while(predict(ZL1_TOKEN_RBRACE, lex))
    {
        if(predict(ZL1_TOKEN_EOF, lex) == success)
            zlerror("unexpected end of file", NULL);

        struct SEXPR* node = zlmalloc(sizeof(struct SEXPR));
        node->car = zlmalloc(sizeof(struct SEXPR));

        if(parse_stmt(node->car, lex));
            zlerror("expected statement or closing brace", NULL);

        tail->cdr = node;
        tail = node;
    }
    return success;
}


int parse_expr(struct SEXPR* expr, lexer_t* lex)
{
    zlassert(lex, "parse_expr( NULL )");
    
    if(predict(ZL1_TOKEN_LPAREN, lex) == success)
    {
        if(parse_expr(expr, lex) == failure)
            zlerror("expected expression", NULL);

        if(predict(ZL1_TOKEN_RPAREN, lex) == failure)
            zlerror("expected closing parenthesis", NULL);

        consume(lex);
    }
    else if(parse_atom(expr, lex) && parse_list(expr, lex))
    {
        return failure;
    }

    if(predict(ZL1_TOKEN_LPAREN, lex) == success)
    {
        struct SEXPR* func = expr;
        expr = zlmalloc(sizeof(struct SEXPR));
        expr->car = func;
        
        expr->type = NULL; // case: f :: i32 ();
        expr->flag = AST_FLAG_CONS | AST_FLAG_CALL;

        if(predict(ZL1_TOKEN_RPAREN, lex) == success)
        {
            expr->cdr = zlmalloc(sizeof(struct SEXPR));
            expr->cdr->flag = AST_FLAG_NIL;
        }
        else
        {
            struct SEXPR* tail = zlmalloc(sizeof(struct SEXPR));
            expr->cdr = tail; // cons(f ...)

            tail->car = zlmalloc(sizeof(struct SEXPR));

            if(parse_expr(tail->car, lex) == failure)
                zlerror("expected expression or closing parenthesis", NULL);

            struct TOKEN* tok;
            while(predict(ZL1_TOKEN_RPAREN, lex) == failure)
            {
                if(predict(ZL1_TOKEN_EOF, lex))
                    zlerror("unexpected end of file", NULL);

                struct SEXPR* next = zlmalloc(sizeof(struct SEXPR));
                next->car = zlmalloc(sizeof(struct SEXPR));

                if(parse_expr(next->car, lex) == failure)
                    zlerror("expected expression or closing parenthesis", NULL);

                tail->cdr = next;
                tail = next;
            }
        }
    }
    
    if(predict(ZL1_TOKEN_COLONS, lex) == success)
    {
        if(lookahead(lex)->tag == ZL1_TOKEN_SYMBOL)
            zlerror("expected type symbol", NULL);

        expr->type = lookahead(lex)->val;
        consume(lex);
    }
    return success;
}
