#ifndef __PARSER_H_
#define __PARSER_H_

#include "ast.h"

typedef struct lexer_s lexer_t;

struct SEXPR* parse(lexer_t* lex);
int parse_atom(struct SEXPR*, lexer_t* lex);
int parse_stmt(struct SEXPR*, lexer_t* lex);
int parse_list(struct SEXPR*, lexer_t* lex);
int parse_expr(struct SEXPR*, lexer_t* lex);

#endif
