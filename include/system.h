#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <types.h>

#define kassert(x) \
        ((void)((!!(x)) || (__kassert(#x, __FILE__, __LINE__, __func__), 0)))

void __kassert(const char *expr, const char *file, int line, const char *func);
void panic(const char* fmt, ...);

#endif
 
