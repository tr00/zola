#include <stdlib.h>
#include <ctype.h>

#include "ZL0.h"
#include "ZL1.h"

typedef struct lexer_s {
    size_t lineno;
    char *filename, *src;
    token_t* next;
} lexer_t;

static void ZL1_next__(lexer_t* lex, token_t* tok)
{
#ifndef NDEBUG
    if(!lex || !tok)
    {
        ZL0_fatal("ZL1_next__( NULL , NULL )");
    }
#endif
    tok->tag = TT_EOF;
    // skip spaces
    while(*lex->src != '\0' && isspace(*lex->src))
    {
        if(*lex->src == '\n')
        {
            lex->lineno++;
        }
        lex->src++;
    }

    char c = *lex->src;

    if(isdigit(c))
    {
        size_t len = 1;
        while(isdigit(*(lex->src + len)))
        {
            len++;
        }
        tok->tag = TT_INTEGER;
        tok->val = (char*) ZL0_malloc(sizeof(char) * (len + 1));
        ZL0_strncpy(tok->val, lex->src, len + 1);

        lex->src += len;
    }
    else if(isalnum(c) || c == '_')
    {
        size_t len = 1;
        while(isalnum(*(lex->src + len)) || *(lex->src + len) == '_')
        {
            len++;
        }
        tok->tag = TT_SYMBOL;
        tok->val = (char*) ZL0_malloc(sizeof(char) * (len + 1));

        ZL0_strncpy(tok->val, lex->src, len + 1);

        lex->src += len;
    }
    else
    {
        switch(c)
        {
            case '(': { tok->tag = TT_LPAREN; } break;
            case ')': { tok->tag = TT_RPAREN; } break;
            case '\0': break;
            default:
                // fprintf(stderr, "unknown character '\\%x'\n", *lex->src);
                break;
        }
        lex->src++;
    }
}

token_t* ZL1_consume(lexer_t* lex)
{

    if(lex)
    {
        token_t* tok = lex->next; /* safe */
        lex->next = ZL0_malloc(sizeof(token_t));/* safe */
        ZL1_next__(lex, lex->next);

#ifdef __TRACE_LEXER
        static int count = 1;
        if(tok)
        {
            printf("__lex_consume( ... ) # %d -> ", count);
        }
        else
        {
            printf("__lex_consume( ... ) # %d -> NULL\n", count);
        }
        count++;
#endif

        return tok;
    }
    ZL0_fatal("ZL1_consume( NULL )");
}

token_t* ZL1_lookahead(lexer_t* lex)
{
#ifndef NDEBUG
    if(lex == NULL)
    {
        ZL0_fatal("ZL1_lookahead( NULL )");
    }
#endif

#ifdef __TRACE_LEXER
    static int count = 1;
    printf("__lex_lookahead( ... ) # %d -> ", count++);
    __tok_print(*lex->next);
#endif

    return lex->next;
}

lexer_t* ZL1_create(char* src, char* filename)
{
    lexer_t* lex = (lexer_t*) ZL0_malloc(sizeof(lexer_t)); /* safe */

    lex->src = src; /* unsafe */
    lex->filename = filename; /* unsafe */
    lex->lineno = 1;

    lex->next = ZL0_malloc(sizeof(token_t)); /* safe */
    ZL1_next__(lex, lex->next);

    return lex;
}

void ZL1_free(lexer_t* lex)
{
#ifdef __TRACE_LEXER
    printf("ZL1_free( ... )\n");
#endif
    if(lex)
    {
        if(lex->next)
        {
            free(lex->next);
        }
        free(lex);
    }
}

