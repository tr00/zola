
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "types.h"
#include "parser.h"
#include "scanner.h"
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

struct AST_LITERAL* parse_literal(lexer_t* lex)
{
    ZL0_assert(lex, "parse_literal( NULL )");
    struct TOKEN* tok = ZL1_lookahead(lex);
    if(tok->tag == ZL1_TOKEN_INTEGER)
    {
        struct AST_LITERAL* atom = ZL0_malloc(sizeof(struct AST_LITERAL));
        atom->val = tok->val;
        atom->type = TYPE_I64;
        free(ZL1_consume(lex));
        return atom;
    }
    return NULL;
}

struct AST_SYMBOL* parse_symbol(lexer_t* lex)
{
    ZL0_assert(lex, "parse_symbol( NULL )");
    struct TOKEN* tok = ZL1_lookahead(lex);
    if(tok->tag == ZL1_TOKEN_SYMBOL)
    {
        struct AST_SYMBOL* atom = ZL0_malloc(sizeof(struct AST_SYMBOL));
        atom->val = tok->val;
        free(ZL1_consume(lex));
        return atom;
    }
    return NULL;
}

struct AST_NODE* parse_statement(lexer_t* lex)
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

struct AST_LIST* parse_list(lexer_t* lex)
{
    ZL0_assert(lex, "ZL2_parse_list( NULL )");

#ifdef __TRACE_PARSER
    printf("ZL2_parse_list( ... ) start\n");
#endif

    struct TOKEN* tok = ZL1_lookahead(lex);

    if(tok->tag == ZL1_TOKEN_LBRACE)
    {
        free(ZL1_consume(lex));
        struct AST_LIST* head = ZL0_malloc(sizeof(struct AST_LIST));

        tok = ZL1_lookahead(lex);
        if(tok->tag == ZL1_TOKEN_RBRACE)
        {
            free(ZL1_consume(lex));
            head->node = NULL;
            return head;
        }

        head->node = parse_statement(lex);
        ZL0_assert(head->node, "expected statement or semicolon");

        tok = ZL1_lookahead(lex);
        struct AST_LIST* tail = head;

        /* this section does one unnecessary malloc + free */
        while(tok->tag != ZL1_TOKEN_RBRACE)
        {
            ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

            struct AST_LIST* node = ZL0_malloc(sizeof(struct AST_LIST));

            node->node = parse_statement(lex);
            ZL0_assert(node->node, "expected stamtement or closing brace");

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

    if((expr->val.list = parse_list(lex)) != NULL)
    {
        expr->tag = AST_NODE_BLOCK;
    }
    else if((expr->val.symbol = parse_symbol(lex)) != NULL)
    {
        expr->tag = AST_NODE_SYMBOL;
    }
    else if((expr->val.literal = parse_literal(lex)) != NULL)
    {
        expr->tag = AST_NODE_LITERAL;
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
        struct AST_LIST* call = ZL0_malloc(sizeof(struct AST_LIST));
        call->node = expr;
        expr = ZL0_malloc(sizeof(struct AST_NODE));
        expr->val.list = call;
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
            struct AST_LIST* head = ZL0_malloc(sizeof(struct AST_LIST));
            call->next = head;
            head->node = parse_expr(lex);
            ZL0_assert(head->node, "expected expression or closing parenthesis");

            struct AST_LIST* tail = head;
            while((tok = ZL1_lookahead(lex))->tag != ZL1_TOKEN_RPAREN)
            {
                ZL0_assert(tok->tag != ZL1_TOKEN_EOF, "unexpected end of file");

                struct AST_LIST* node = ZL0_malloc(sizeof(struct AST_LIST));

                node->node = parse_expr(lex);
                ZL0_assert(node->node, "expected expression or closing parenthesis");

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

static void print_list(struct AST_LIST* list, int tc)
{
    while(list->node)
    {
        print_ast(list->node, tc);
        if(list->next)
        {
            list = list->next;
        }
        else
        {
            break;
        }
    }
}


void print_ast(struct AST_NODE* node, int tc)
{
    switch(node->tag)
    {
    case AST_NODE_CALL:
    case AST_NODE_BLOCK:
        printf("%sBLOCK[\n", tabs(tc));
        tc++;
        print_list(node->val.list, tc);
        tc--;
        printf("%s]\n", tabs(tc));
        break;
    case AST_NODE_LITERAL:
        printf("%sINT[%s]\n", tabs(tc), node->val.literal->val);
        break;
    case AST_NODE_SYMBOL:
        printf("%sSYMBOL[%s]\n", tabs(tc), node->val.symbol->val);
        break;
    default:
        break;
    }
}

/*

void ZL2_print_atom(struct ZL2_AST_ATOM atom)
{
    printf("atom[%d|%s]", atom.tag, atom.val ? atom.val : "NULL");
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
*/



