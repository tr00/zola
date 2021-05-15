#ifndef __PARSER_H_
#define __PARSER_H_

#include "ast.h"

typedef struct lexer_s lexer_t;

struct SEXPR* parse_atom(lexer_t* lex);
struct SEXPR* parse_stmt(lexer_t* lex);
struct SEXPR* parse_list(lexer_t* lex);
struct SEXPR* parse_expr(lexer_t* lex);

#endif
