#ifndef __ERRORS_H_
#define __ERRORS_H_

#ifndef NDEBUG
#define zlassert(cond, msg) if(!(cond)) zlcrash(msg, __FILE__, __LINE__);
#else
#define zlassert(cond, msg)
#endif
#define zlfatal(msg) zlcrash(msg, __FILE__, __LINE__);

void __attribute__((noreturn, noinline)) zlcrash(const char*, const char*, int);
void __attribute__((noreturn, noinline)) zlerror(const char*, void*);

#endif