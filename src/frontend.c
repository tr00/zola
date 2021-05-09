// Zola

// #define __TRACE_LEXER
// #define __TRACE_PARSER
// #define __TRACE_EVAL

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "ZL0.h"
#include "ZL1.h"
#include "ZL2.h"
#include "ZL3.h"

// types & structs


// globals

// utils

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

    while(ZL1_lookahead(lex)->tag != TT_EOF)
    {
        token_t* tok = ZL1_consume(lex);
        printf("[DEBUG]\n");
        free(tok);
    }

    ZL1_free(lex);
}


int main(int argc, char **argv)
{
    lexer_t* lex = ZL1_create("(putc 65)", "<unknown>");
    
    expr_t* expr = ZL2_expr(lex);


    value_t* value = ZL3_expr(expr, NULL);

    if(value && value->tag == VT_CONSTANT)
        printf("value: %d\n", value->val.__constant.val);

    free(expr);
    ZL1_free(lex);
    return 0;
}
