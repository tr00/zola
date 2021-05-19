// Zola

// #define __TRACE_PARSER
// #define __TRACE_EVAL

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "frontend.h"
#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "semantics.h"
#include "errors.h"


/**
 * frontend optimizations:
 *      compile time execution
 *      inlining
 * 
 * backend optimizations:
 *      sparse const propagation
 *      copy elimination
 *      dead instruction elimination
 */

void __attribute__((noreturn, noinline)) zlcrash(const char *msg, const char* file, int line)
{
    // memory managing is done by the OS
    fprintf(stderr, "compiler-crash: %s\n", msg);
    exit(-1);
}

void __attribute__((noreturn, noinline)) zlerror(const char* msg, void* pos)
{
    fprintf(stderr, "syntax error: %s\n", msg);
    exit(-1);
}

/**
 * a function that acts like malloc but which terminates
 * if malloc fails to return a valid pointer
 */
void* __attribute__((always_inline)) zlmalloc(size_t size)
{
    void* ptr = malloc(size);
    zlassert(ptr, "out of dynamic memory!");
    return ptr;
}

/**
 * a custom strncpy function which ensures
 * that the returned string is \0 terminated
 */
char* zlstrncpy(char* dest, char* src, size_t n)
{
    zlassert(n > 0, "zlstrncpy( ..., -1)");
    size_t i;
    for(i = 0; i < n - 1 && src[i]; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return dest;
}

char* __attribute__((always_inline)) zlstrdup(char *src)
{
    char* dup = zlmalloc(strlen(src) + 1);
    strcpy(dup, src);
    return dup;
}


// driver

void debug_lexer(char *src)
{
    lexer_t* lex = ZL1_create(src, "<debug>");

    while(lookahead(lex)->tag != ZL1_TOKEN_EOF)
    {
        struct TOKEN* tok = lookahead(lex);
        consume(lex);
        printf("[LEXER] >> %d\n", tok->tag);
        free(tok);
    }

    ZL1_free(lex);
}

void debug_parser(char *src)
{
    lexer_t* lex = ZL1_create(src, "<debug>");

    struct SEXPR* expr = zlmalloc(sizeof(struct SEXPR));
    parse_expr(expr, lex);

    printf("\n");

    free(expr);
    ZL1_free(lex);
}

static inline void ptabs(int c)
{
    for(int i = 0; i < c; i++)
        printf("   ");
}

static void dump_ast(struct SEXPR* node)
{
    if(node->flag & AST_FLAG_ATOM)
    {
        // printf("atom: %p\n", node);
        printf("%s ", node->atom);
    }
    else if(node->flag & AST_FLAG_CONS)
    {
        printf("cons(");
        if(node->car)
        {
            dump_ast(node->car);
        }
        if(node->cdr)
            dump_ast(node->cdr);
        else
        {
            printf("()");
        }
        printf(")");
    }
    else if(!node->flag)
    {
        // printf("flag: %d", node->flag);
        printf("nil ");
    }
    else 
    {
        printf("oops!");
    }
}

/**
 * TODO:
 *  - DO NOT edit the ast
 *  - implement dispatcher
 *  - finish simple codegen
 *  - add error messages
 *  - add macros
 */

int main(int argc, char **argv)
{
    //debug_parser("{ def!; 38239; f(); }");
    lexer_t* lex = ZL1_create("f( () 443)", "<unknown>");
    
    struct SEXPR* node = zlmalloc(sizeof(struct SEXPR));
    parse_expr(node, lex);
    // visit_node(node, NULL);

    dump_ast(node);
    printf("\n");
    // printf("codegen...\n");
    // codegen(node);    

    free(node);
    ZL1_free(lex);
    return 0;
}
