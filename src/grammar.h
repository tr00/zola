/**
 * The Parser and its utilities
 */
#ifndef __ZL2_H_
#define __ZL2_H_

#include "ast.h"

typedef struct lexer_s lexer_t;


struct AST_LITERAL* parse_literal(lexer_t* lex);
struct AST_SYMBOL* parse_symbol(lexer_t* lex);
struct AST_NODE* parse_statement(lexer_t* lex);
struct AST_LIST* parse_list(lexer_t* lex);
struct AST_NODE* parse_expr(lexer_t* lex);

void print_ast(struct AST_NODE*, int);

#endif
