#ifndef __FRONTEND_H_
#define __FRONTEND_H_

#include <stddef.h>

#ifndef NDEBUG
#define ZL0_assert(cond, msg) if(!(cond)) ZL0_fatal((msg));
#else
#define ZL0_assert(cond, msg) ;
#endif

/**
 *
 */
void __attribute__((__noreturn__)) ZL0_fatal(const char *);

/**
 *
 */
void ZL0_trace(char *);

/**
 *
 */
void* ZL0_malloc(size_t);
#define zlmalloc(size) ZL0_malloc(size);

/**
 *
 */
char* ZL0_strncpy(char*, char*, size_t);

char* ZL0_strdup(char*);
#define zlstrdup(str) ZL0_strdup(str);

#endif

