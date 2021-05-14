
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
 *      f()
 *      { f; }()
 *      (g())()
 */
;

struct AST_ATOM* parse_atom(lexer_t* lex)
{
    ZL0_assert(lex, "parse_atom( NULL )");
    struct TOKEN* tok = ZL1_lookahead(lex);
    if(tok->tag == ZL1_TOKEN_INTEGER || tok->tag == ZL1_TOKEN_SYMBOL)
    {
        struct AST_ATOM* atom = ZL0_malloc(sizeof(struct AST_ATOM));
        atom->val = tok->val;
        atom->tag = tok->tag == ZL1_TOKEN_SYMBOL ? AST_ATOM_VARIABLE : AST_ATOM_LITERAL;
        // atom->type == TYPE_I64
        free(ZL1_consume(lex));
        return atom;
    }
    return NULL;
}

struct AST_NODE* parse_stmt(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_stmt( NULL )");
    struct AST_NODE* expr = parse_expr(lex);

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

struct AST_NODE* parse_list(lexer_t* lex)
{
    ZL0_assert(lex, "parse_list( NULL )");

#ifdef __TRACE_PARSER
    printf("ZL2_parse_list( ... ) start\n");
#endif

    struct TOKEN* tok = ZL1_lookahead(lex);

    if(tok->tag == ZL1_TOKEN_LBRACE)
    {
        free(ZL1_consume(lex));
        struct AST_NODE* head = zlmalloc(sizeof(struct AST_NODE));

        tok = ZL1_lookahead(lex);
        if(tok->tag == ZL1_TOKEN_RBRACE)
        {
            free(ZL1_consume(lex));
            return head;
        }

        head->val.node = parse_stmt(lex);
        ZL0_assert(head->val.node, "expected statement or semicolon");

        tok = ZL1_lookahead(lex);
        struct AST_NODE* tail = head;

        /* this section does one unnecessary malloc + free */
        while(tok->tag != ZL1_TOKEN_RBRACE)
        {
            ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

            struct AST_NODE* node = ZL0_malloc(sizeof(struct AST_NODE));

            node->val.node = parse_stmt(lex);
            ZL0_assert(node->val.node, "expected stamtement or closing brace");

            tail->next = node;
            tail = node;
            tok = ZL1_lookahead(lex);
        }

        free(ZL1_consume(lex));
        return head;
    }

    return NULL;
}


struct AST_NODE* parse_expr(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_expr( NULL )");
    
    struct AST_NODE* expr = ZL0_malloc(sizeof(struct AST_NODE));

    if((expr->val.node = parse_list(lex)) != NULL)
    {
        expr->tag = AST_NODE_BLOCK;
    }
    else if((expr->val.atom = parse_atom(lex)) != NULL)
    {
        expr->tag = AST_NODE_ATOM;
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
        return NULL;
    }

    struct TOKEN* tok = ZL1_lookahead(lex); 
    if(tok->tag == ZL1_TOKEN_LPAREN)
    {
        free(ZL1_consume(lex));
        struct AST_NODE* call = ZL0_malloc(sizeof(struct AST_NODE));
        call->val.node = expr;
        expr = ZL0_malloc(sizeof(struct AST_NODE));
        expr->val.node = call;
        expr->type = NULL;
        expr->tag = AST_NODE_CALL;

        tok = ZL1_lookahead(lex);
        if(tok->tag == ZL1_TOKEN_RPAREN)
        {
            free(ZL1_consume(lex));
            call->next = NULL;
        }
        else
        {
            struct AST_NODE* head = ZL0_malloc(sizeof(struct AST_NODE));
            call->next = head;
            head->val.node = parse_expr(lex);
            ZL0_assert(head->val.node, "expected expression or closing parenthesis");

            struct AST_NODE* tail = head;
            while((tok = ZL1_lookahead(lex))->tag != ZL1_TOKEN_RPAREN)
            {
                ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

                struct AST_NODE* node = ZL0_malloc(sizeof(struct AST_NODE));

                node->val.node = parse_expr(lex);
                ZL0_assert(node->val.node, "expected expression or closing parenthesis");

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

static char* tabs(int tc)
{
    char *str = malloc(sizeof(char) * (tc + 1));
    for(int i = 0; i < tc; i++)
    {
        str[i] = '\t';
    }
    str[tc] = '\0';
    return str;
}


void print_ast(struct AST_NODE* node, int tc)
{
    switch(node->tag)
    {
    case AST_NODE_CALL:
        printf("call\n");
        break;
    case AST_NODE_BLOCK:
        printf("block\n");
        break;
    case AST_NODE_ATOM:
        printf("atom\n");
        break;
    default:
        break;
    }
}