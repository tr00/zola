#ifndef __ZL0_H_
#define __ZL0_H_

#include <stddef.h>

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

/**
 *
 */
char* ZL0_strncpy(char*, char*, size_t);

#endif

