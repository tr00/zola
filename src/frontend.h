#ifndef __FRONTEND_H_
#define __FRONTEND_H_

#include <stddef.h>

#define success 0
#define failure 1

/**
 *
 */
void ZL0_trace(char *);

/**
 *
 */
void* __attribute__((always_inline)) zlmalloc(size_t);

/**
 *
 */
char* zlstrncpy(char*, char*, size_t);

char* __attribute__((always_inline)) zlstrdup(char*);

#endif

