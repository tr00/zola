#ifndef __PARSER_H_
#define __PARSER_H_

#include "ast.h"

typedef struct lexer_s lexer_t;

struct AST_ATOM* parse_atom(lexer_t* lex);
struct AST_NODE* parse_stmt(lexer_t* lex);
struct AST_NODE* parse_list(lexer_t* lex);
struct AST_NODE* parse_expr(lexer_t* lex);

void print_ast(struct AST_NODE*, int);

#endif
