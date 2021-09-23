// SPDX-License-Identifier: GPL-2.0
/*
 * Most of the string-functions are rather heavily hand-optimized,
 * see especially strsep,strstr,str[c]spn. They should work, but are not
 * very easy to understand. Everything is done entirely within the register
 * set, making the functions fast and clean. String instructions have been
 * used through-out, making for "slightly" unclear code :-)
 *
 * AK: On P4 and K7 using non string instruction implementations might be faster
 * for large memory blocks. But most of them are unlikely to be used on large
 * strings.
 */

#include <string.h>
#include <export.h>

char *strcpy(char *dest, const char *src)
{
    int d0, d1, d2;
    asm volatile("1:\tlodsb\n"
        "stosb\n"
        "testb %%al,%%al\n"
        "jne 1b"
        : "=&S" (d0), "=&D" (d1), "=&a" (d2)
        : "0" (src), "1" (dest) : "memory");
    return dest;
}
EXPORT_SYMBOL(strcpy);

char *strncpy(char *dest, const char *src, size_t count)
{
    int d0, d1, d2, d3;

    asm volatile ("1:\tdecl %2\n"
        "js 2f\n"
        "lodsb\n"
        "stosb\n"
        "testb %%al,%%al\n"
        "jne 1b\n"
        "rep\n"
        "stosb\n"
        "2:"
        :"=&S"(d0), "=&D"(d1), "=&c"(d2), "=&a"(d3)
        :"0"(src), "1"(dest), "2"(count) :"memory"
    );

    return dest;
}
EXPORT_SYMBOL(strncpy);

char *strcat(char *dest, const char *src)
{
    int d0, d1, d2, d3;

    asm volatile(
        "repne\n"
        "scasb\n"
        "decl %1\n"
        "1:\n"
        "lodsb\n"
        "stosb\n"
        "testb %%al,%%al\n"
        "jne 1b"
        :"=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
        :"0"(src), "1"(dest), "2"(0), "3"(0xffffffffu)
        :"memory"
    );

    return dest;
}
EXPORT_SYMBOL(strcat);

char *strncat(char *dest, const char *src, size_t count)
{
    int d0, d1, d2, d3;

    asm volatile(
        "repne\n"
        "scasb\n"
        "decl %1\n"
        "movl %8,%3\n"
        "1:\n"
        "decl %3\n"
        "js 2f\n"
        "lodsb\n"
        "stosb\n"
        "testb %%al,%%al\n"
        "jne 1b\n"
        "2:\n"
        "xorl %2,%2\n"
        "stosb"
        :"=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
        :"0"(src), "1"(dest), "2"(0), "3"(0xffffffffu), "g"(count)
        :"memory"
    );

    return dest;
}
EXPORT_SYMBOL(strncat);

int strcmp(const char *cs, const char *ct)
{
    int d0, d1, res;

    asm volatile (
        "1:\n"
        "lodsb\n"
        "scasb\n"
        "jne 2f\n"
        "testb %%al,%%al\n"
        "jne 1b\n"
        "xorl %%eax,%%eax\n"
        "jmp 3f\n"
        "2:\n"
        "sbbl %%eax,%%eax\n"
        "orb $1,%%al\n"
        "3:"
        :"=a"(res), "=&S"(d0), "=&D"(d1)
        :"1"(cs), "2"(ct)
        :"memory"
    );

    return res;
}
EXPORT_SYMBOL(strcmp);

int strncmp(const char *cs, const char *ct, size_t count)
{
    int res, d0, d1, d2;

    asm volatile (
        "1:\n"
        "decl %3\n"
        "js 2f\n"
        "lodsb\n"
        "scasb\n"
        "jne 3f\n"
        "testb %%al,%%al\n"
        "jne 1b\n"
        "2:\n"
        "xorl %%eax,%%eax\n"
        "jmp 4f\n"
        "3:\n"
        "sbbl %%eax,%%eax\n"
        "orb $1,%%al\n"
        "4:"
        : "=a" (res), "=&S" (d0), "=&D" (d1), "=&c" (d2)
        : "1" (cs), "2" (ct), "3" (count)
        : "memory"
    );

    return res;
}
EXPORT_SYMBOL(strncmp);

char *strchr(const char *s, int c)
{
    int d0;
    char *res;

    asm volatile (
        "movb %%al,%%ah\n"
        "1:\tlodsb\n"
        "cmpb %%ah,%%al\n"
        "je 2f\n"
        "testb %%al,%%al\n"
        "jne 1b\n"
        "movl $1,%1\n"
        "2:\tmovl %1,%0\n"
        "decl %0"
        : "=a" (res), "=&S" (d0)
        : "1" (s), "0" (c)
        : "memory"
    );

    return res;
}
EXPORT_SYMBOL(strchr);

size_t strlen(const char *s)
{
    int d0;
    size_t res;

    asm volatile (
        "repne\n"
        "scasb"
        : "=c" (res), "=&D" (d0)
        : "1" (s), "a" (0), "0" (0xffffffffu)
        : "memory"
    );

    return ~res - 1;
}
EXPORT_SYMBOL(strlen);

void *memchr(const void *cs, int c, size_t count)
{
    int d0;
    void *res;

    if (!count)
        return NULL;

    asm volatile (
        "repne\n"
        "scasb\n"
        "je 1f\n"
        "movl $1,%0\n"
        "1:\tdecl %0"
        : "=D" (res), "=&c" (d0)
        : "a" (c), "0" (cs), "1" (count)
        : "memory"
    );

    return res;
}
EXPORT_SYMBOL(memchr);

void *memscan(void *addr, int c, size_t size)
{
    if (!size)
        return addr;

    asm volatile (
        "repnz; scasb\n"
        "jnz 1f\n"
        "dec %%edi\n"
        "1:"
        :"=D"(addr), "=c"(size)
        :"0"(addr), "1"(size), "a"(c)
        :"memory"
    );

    return addr;
}
EXPORT_SYMBOL(memscan);

size_t strnlen(const char *s, size_t count)
{
    int d0;
    int res;

    asm volatile (
        "movl %2,%0\n"
        "jmp 2f\n"
        "1:\tcmpb $0,(%0)\n"
        "je 3f\n"
        "incl %0\n"
        "2:\tdecl %1\n"
        "cmpl $-1,%1\n"
        "jne 1b\n"
        "3:\tsubl %2,%0"
        :"=a"(res), "=&d"(d0)
        :"c"(s), "1"(count)
        :"memory"
    );

    return res;
}
EXPORT_SYMBOL(strnlen);
