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


// driver

void debug_lexer(char *src)
{
    lexer_t* lex = ZL1_create(src, "<debug>");

    while(ZL1_lookahead(lex)->tag != TT_EOF)
    {
        token_t* tok = ZL1_consume(lex);
        printf("[DEBUG] ");
        free(tok);
    }

    ZL1_free(lex);
}


int main(int argc, char **argv)
{
    // debug_lexer("(test)"); 
    
 
    return 0;
}
