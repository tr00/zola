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

// #include "ZL3.h"

void __attribute__((__noreturn__)) ZL0_fatal(const char *msg)
{
    // memory managing is done by the OS
    fprintf(stderr, "error: %s\n", msg);
    exit(-1);
}

/**
 * a function that acts like malloc but which terminates
 * if malloc fails to return a valid pointer
 */
void* ZL0_malloc(size_t size)
{
    void* ptr = malloc(size);

    if(ptr == NULL)
    {
        ZL0_fatal("out of dynamic memory");
    }

    return ptr;
}

/**
 * a custom strncpy function which ensures
 * that the returned string is \0 terminated
 */
char* ZL0_strncpy(char* dest, char* src, size_t n)
{
#ifndef NDEBUG
    if(n <= 0)
    {
        ZL0_fatal("__strncpy( ... , -1 )");
    }
#endif
    size_t i;
    for(i = 0; i < n - 1 && src[i]; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';

#ifndef NDEBUG
        if(strlen(dest) > n)
        {
            fprintf(stderr, "assertion failed: __strncpy( ... )\n");
        }
#endif
    return dest;
}

char* ZL0_strdup(char *src)
{
    char* dup = ZL0_malloc(strlen(src) + 1);
    strcpy(dup, src);
    return dup;
}


// driver

void debug_lexer(char *src)
{
    lexer_t* lex = ZL1_create(src, "<debug>");

    while(ZL1_lookahead(lex)->tag != ZL1_TOKEN_EOF)
    {
        struct TOKEN* tok = ZL1_consume(lex);
        printf("[LEXER] >> %d\n", tok->tag);
        free(tok);
    }

    ZL1_free(lex);
}

void debug_parser(char *src)
{
    lexer_t* lex = ZL1_create(src, "<debug>");

    struct AST_NODE* expr = parse_expr(lex);

    print_ast(expr, 0);
    printf("\n");

    free(expr);
    ZL1_free(lex);
}


int main(int argc, char **argv)
{
    //debug_parser("{ def!; 38239; f(); }");
    lexer_t* lex = ZL1_create("__add__(1 2)", "<unknown>");
    
    struct AST_NODE* node = parse_expr(lex);
    visit_node(node, NULL);

    printf("codegen...\n");
    codegen(node);    

    free(node);
    ZL1_free(lex);
    return 0;
}
