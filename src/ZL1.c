#include <stdlib.h>
#include <ctype.h>

#include "ZL0.h"
#include "ZL1.h"

typedef struct lexer_s {
    size_t lineno;
    char *filename, *src;
    struct ZL1_TOKEN* next;
} lexer_t;

/**
 *      LPAREN ::= \(
 *      RPAREN ::= \)
 *      LBRACE ::= \{
 *      RBRACE ::= \}
 *      COLONS ::= \:\:
 *      INTEGER ::= \d+
 *      SYMBOL ::= [a-zA-Z0-9_]+
 *
 */

static int symbol_pattern(char c)
{
    return !(isspace(c) || c == '\0' ||  c == '(' || c == ')' || c == '{' || c == '}' || c == ':');
}

static void ZL1_next__(lexer_t* lex, struct ZL1_TOKEN* tok)
{
    ZL0_assert(lex && tok, "ZL1_next__( NULL , NULL )");
    tok->tag = ZL1_TOKEN_EOF;
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
        tok->tag = ZL1_TOKEN_INTEGER;
        tok->val = (char*) ZL0_malloc(sizeof(char) * (len + 1));
        ZL0_strncpy(tok->val, lex->src, len + 1);

        lex->src += len;
    }
    else if(c == ':')
    {
        if(*(lex->src + 1) == ':')
        {
            tok->tag = ZL1_TOKEN_COLONS;
            lex->src += 2;
        }
    }
    else if(c == '(')
    {
        tok->tag = ZL1_TOKEN_LPAREN;
        lex->src++;
    }
    else if(c == ')')
    {
        tok->tag = ZL1_TOKEN_RPAREN;
        lex->src++;
    }
    else if(c == '{')
    {
        tok->tag = ZL1_TOKEN_LBRACE;
        lex->src++;
    }
    else if(c == '}')
    {
        tok->tag = ZL1_TOKEN_RBRACE;
        lex->src++;
    }
    else if(symbol_pattern(c))
    {
        size_t len = 1;
        while(symbol_pattern(*(lex->src + len)))
        {
            len++;
        }
        tok->tag = ZL1_TOKEN_SYMBOL;
        tok->val = (char*) ZL0_malloc(sizeof(char) * (len + 1));

        ZL0_strncpy(tok->val, lex->src, len + 1);

        lex->src += len;
    }
}

struct ZL1_TOKEN* ZL1_consume(lexer_t* lex)
{

    if(lex)
    {
        struct ZL1_TOKEN* tok = lex->next; /* safe */
        lex->next = ZL0_malloc(sizeof(struct ZL1_TOKEN));/* safe */
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

struct ZL1_TOKEN* ZL1_lookahead(lexer_t* lex)
{
    ZL0_assert(lex, "ZL1_lookahead( NULL )");

#ifdef __TRACE_LEXER
    static int count = 1;
    printf("__lex_lookahead( ... ) # %d -> ", count++);
    __tok_print(*lex->next);
#endif

    return lex->next;
}

lexer_t* ZL1_create(char* src, char* filename)
{
    lexer_t* lex = (lexer_t*) ZL0_malloc(sizeof(lexer_t)); 

    lex->src = src; /* unsafe */
    lex->filename = filename; /* unsafe */
    lex->lineno = 1;

    lex->next = ZL0_malloc(sizeof(struct ZL1_TOKEN)); 
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

