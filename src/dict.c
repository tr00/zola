/**
 * implementation of a simple hashmap which is used for all sorts of variable lookup
 */
#include <string.h>
#include <stdlib.h>

#include "ast.h"
#include "ZL0.h"

#define HASHSIZE 20

typedef struct dict_s {
    struct dict_s* next;
    struct AST_NODE* node;
    char* name;
} DICT;

static unsigned hash(char* name)
{
    unsigned hashval = 0;
    while(*name)
        hashval = 31 * hashval + *name++;
    return hashval % HASHSIZE;
}

static DICT* lookup(DICT** map, char* name)
{
    DICT* np;
    for(np = map[hash(name)]; np; np = np->next)
    {
        if(0 == strcmp(name, np->name))
        {
            return np;
        }
    }
    return NULL;
}

void push(DICT** map, char* name, struct AST_NODE* node)
{
    DICT* np;
    unsigned hashval;
    if((np = lookup(map, name)) == NULL)
    {
        np = zlmalloc(sizeof(DICT));
        np->name = zlstrdup(name);
        hashval = hash(name);
        np->next = map[hashval];
        map[hashval] = np;
    }
    else
    {
        free(np->node);
    }
    np->node = node;
}
