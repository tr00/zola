/**
 * changes to be done:
 *      better tracing
 *      better error messages
 *      unit testing
 *      function renaming
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "frontend.h"
#include "scanner.h"
#include "errors.h"

typedef struct lexer_s {
    size_t lineno;
    char *filename, *src;
    struct TOKEN* next;
} lexer_t;

/**
 *      SEMICOLON ::= \;
 *      LPAREN ::= \(
 *      RPAREN ::= \)
 *      LBRACE ::= \{
 *      RBRACE ::= \}
 *      COLONS ::= \:\:
 *      INTEGER ::= \d+
 *      SYMBOL ::= [^\s]*
 *
 */

static int issymbol(char c)
{
    static const char* viable = "~!?./,<>_+=-:$%%^&\\[]";
    return isalnum(c) || strchr(viable, c);
}

static void ZL1_next__(lexer_t* lex, struct TOKEN* tok)
{
    zlassert(lex && tok, "ZL1_next__( NULL , NULL )");
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
        tok->val = zlmalloc(sizeof(char) * (len + 1));
        zlstrncpy(tok->val, lex->src, len + 1);

        lex->src += len;
    }
    else if(c == ';')
    {
        tok->tag = ZL1_TOKEN_SEMICOLON;
        lex->src++;
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
    else if(issymbol(c))
    {
        size_t len = 1;
        while(issymbol(*(lex->src + len)))
        {
            len++;
        }
        tok->tag = ZL1_TOKEN_SYMBOL;
        tok->val = zlmalloc(sizeof(char) * (len + 1));

        zlstrncpy(tok->val, lex->src, len + 1);

        lex->src += len;
    }
}

void consume(lexer_t* lex)
{
    zlassert(lex, "consume( NULL )");
    free(lex->next); // should be always assigned
    lex->next = zlmalloc(sizeof(struct TOKEN));/* safe */
    ZL1_next__(lex, lex->next);

#ifdef __TRACE_LEXER
#endif
}

struct TOKEN* __attribute__((always_inline)) lookahead(lexer_t* lex)
{
    zlassert(lex, "ZL1_lookahead( NULL )");

#ifdef __TRACE_LEXER
    static int count = 1;
    printf("ZL1_lookahead#%d( ... ) -> ...\n", count++);
#endif

    return lex->next;
}

int predict(int token, lexer_t* lex)
{
    if(lex->next->tag == token)
    {
        free(lex->next);
        ZL1_next__(lex, (lex->next = zlmalloc(sizeof(struct TOKEN))));
        return success;
    }
    return failure;
}

lexer_t* ZL1_create(char* src, char* filename)
{
#ifdef __TRACE_LEXER
    printf("ZL1_create( ... )\n");
#endif
    lexer_t* lex = zlmalloc(sizeof(lexer_t)); 

    lex->src = src; /* unsafe */
    lex->filename = filename; /* unsafe */
    lex->lineno = 1;

    lex->next = zlmalloc(sizeof(struct TOKEN));
    ZL1_next__(lex, lex->next);

    return lex;
}

void ZL1_free(lexer_t* lex)
{
#ifdef __TRACE_LEXER
    printf("ZL1_free( ... )\n");
#endif
    zlassert(lex, "ZL1_free( NULL )");
    free(lex->next);
    free(lex);
}

