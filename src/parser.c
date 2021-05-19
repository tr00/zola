// #define __TRACE_PARSER
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "types.h"
#include "parser.h"
#include "scanner.h"
#include "frontend.h"
#include "errors.h"

struct SEXPR nil = { .flag = 0, .type = "nil_t" };

extern inline int __attribute__((always_inline)) isnil(struct SEXPR* sexpr)
{
    return sexpr->flag == 0;
}

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
 *      list ::= LBRACE stmt* RBRACE
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

struct SEXPR* parse(lexer_t* lex)
{
    return parse_expr(zlmalloc(sizeof(struct SEXPR)), lex);
}

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
#ifdef __TRACE_PARSER
    static int count = 0;
    printf("parse_stmt#%d( ... )\n", ++count);
#endif

    if(parse_expr(stmt, lex))
        return failure;

    if(predict(ZL1_TOKEN_SEMICOLON, lex))
        zlerror("expected semicolon", NULL);

    return success;
}

int parse_list(struct SEXPR* list, lexer_t* lex)
{
    zlassert(lex, "parse_list( NULL )");
#ifdef __TRACE_PARSER
    static int count = 0;
    printf("parse_list#%d( ... )\n", ++count);
#endif

    if(predict(ZL1_TOKEN_LBRACE, lex))
        return failure;

    if(predict(ZL1_TOKEN_RBRACE, lex) == success)
    {
        list->cdr = &nil;
        return success;
    }

    list->car = zlmalloc(sizeof(struct SEXPR));
    list->flag = AST_FLAG_CONS | AST_FLAG_LIST;
    if(parse_stmt(list->car, lex))
        zlerror("expected statement or semicolon", NULL);

    struct SEXPR* tail = list;

    while(predict(ZL1_TOKEN_RBRACE, lex))
    {
        if(predict(ZL1_TOKEN_EOF, lex) == success)
            zlerror("unexpected end of file", NULL);

        struct SEXPR* node = zlmalloc(sizeof(struct SEXPR));
        node->flag = AST_FLAG_CONS | AST_FLAG_LIST;
        node->car = zlmalloc(sizeof(struct SEXPR));

        if(parse_stmt(node->car, lex))
            zlerror("expected statement or closing brace", NULL);
        
        tail->cdr = node;
        tail = node;
    }
    tail->cdr = &nil;
    return success;
}


int parse_expr(struct SEXPR* expr, lexer_t* lex)
{
    zlassert(lex, "parse_expr( NULL )");
#ifdef __TRACE_PARSER
    static int count = 0;
    printf("parse_expr#%d( ... )\n", ++count);
#endif
    
    if(predict(ZL1_TOKEN_LPAREN, lex) == success)
    {
        if(predict(ZL1_TOKEN_RPAREN, lex) == success)
        {
            *expr = nil;
            return success;
        }

        if(parse_expr(expr, lex) == failure)
            zlerror("expected expression", NULL);

        if(predict(ZL1_TOKEN_RPAREN, lex) == failure)
            zlerror("expected closing parenthesis", NULL);

        // consume(lex);
    }
    else if(parse_atom(expr, lex) && parse_list(expr, lex))
    {
        return failure;
    }

    if(predict(ZL1_TOKEN_LPAREN, lex) == success)
    {
        struct SEXPR* func = zlmalloc(sizeof(struct SEXPR));
        memcpy(func, expr, sizeof(struct SEXPR));
        expr->car = func;
        expr->type = NULL; // case: f :: i32 ();
        expr->flag = AST_FLAG_CONS | AST_FLAG_CALL;

        if(predict(ZL1_TOKEN_RPAREN, lex) == success) 
        {
            expr->cdr = &nil;
            return success;
        }
        else
        {
            struct SEXPR* tail = zlmalloc(sizeof(struct SEXPR));
            expr->cdr = tail; // cons(f ...)

            tail->car = zlmalloc(sizeof(struct SEXPR));

            tail->flag = AST_FLAG_CONS;

            if(parse_expr(tail->car, lex) == failure)
                zlerror("expected expression or closing parenthesis", NULL);
            
            while(predict(ZL1_TOKEN_RPAREN, lex))
            {
                if(predict(ZL1_TOKEN_EOF, lex) == success)
                    zlerror("unexpected end of file", NULL);

                struct SEXPR* next = zlmalloc(sizeof(struct SEXPR));
                next->car = zlmalloc(sizeof(struct SEXPR));
                next->flag = AST_FLAG_CONS;

                if(parse_expr(next->car, lex) == failure)
                    zlerror("expected expression or closing parenthesis", NULL);

                tail->cdr = next;
                tail = next;
            }
            tail->cdr = &nil;
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
