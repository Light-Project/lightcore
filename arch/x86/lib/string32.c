/* SPDX-License-Identifier: GPL-2.0 */
#include <string.h>

char *strcpy(char *dest, const char *src)
{
    int d0, d1, d2;

    asm volatile (
        "1:                 \n"
        "lodsb              \n"
        "stosb              \n"
        "testb  %%al,%%al   \n"
        "jne    1b          \n"
        : "=&S"(d0), "=&D"(d1), "=&a"(d2)
        : "0"(src), "1"(dest)
        : "memory"
    );

    return dest;
}

char *strncpy(char *dest, const char *src, size_t count)
{
    int d0, d1, d2, d3;

    asm volatile (
        "1:                 \n"
        "decl   %2          \n"
        "js     2f          \n"
        "lodsb              \n"
        "stosb              \n"
        "testb  %%al,%%al   \n"
        "jne    1b          \n"
        "rep                \n"
        "stosb              \n"
        "2:                 \n"
        : "=&S"(d0), "=&D"(d1), "=&c"(d2), "=&a"(d3)
        : "0"(src), "1"(dest), "2"(count)
        : "memory"
    );

    return dest;
}

char *strcat(char *dest, const char *src)
{
    int d0, d1, d2, d3;

    asm volatile (
        "repne              \n"
        "scasb              \n"
        "decl   %1          \n"
        "1:                 \n"
        "lodsb              \n"
        "stosb              \n"
        "testb  %%al, %%al  \n"
        "jne    1b          \n"
        : "=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
        : "0"(src), "1"(dest), "2"(0), "3"(0xffffffffu)
        : "memory"
    );

    return dest;
}

char *strncat(char *dest, const char *src, size_t count)
{
    int d0, d1, d2, d3;

    asm volatile (
        "repne              \n"
        "scasb              \n"
        "decl   %1          \n"
        "movl   %8, %3      \n"
        "1:                 \n"
        "decl   %3          \n"
        "js     2f          \n"
        "lodsb              \n"
        "stosb              \n"
        "testb  %%al, %%al  \n"
        "jne    1b          \n"
        "2:                 \n"
        "xorl   %2, %2      \n"
        "stosb              \n"
        : "=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
        : "0"(src), "1"(dest), "2"(0), "3"(0xffffffffu), "g"(count)
        : "memory"
    );

    return dest;
}

int strcmp(const char *cs, const char *ct)
{
    int d0, d1, res;

    asm volatile (
        "1:                     \n"
        "lodsb                  \n"
        "scasb                  \n"
        "jne    2f              \n"
        "testb  %%al, %%al      \n"
        "jne    1b              \n"
        "xorl   %%eax, %%eax    \n"
        "jmp    3f              \n"
        "2:                     \n"
        "sbbl   %%eax, %%eax    \n"
        "orb    $1, %%al        \n"
        "3:                     \n"
        : "=a"(res), "=&S"(d0), "=&D"(d1)
        : "1"(cs), "2"(ct)
        : "memory"
    );

    return res;
}

int strncmp(const char *cs, const char *ct, size_t count)
{
    int res, d0, d1, d2;

    asm volatile (
        "1:                     \n"
        "decl   %3              \n"
        "js     2f              \n"
        "lodsb                  \n"
        "scasb                  \n"
        "jne    3f              \n"
        "testb  %%al, %%al      \n"
        "jne    1b              \n"
        "2:                     \n"
        "xorl   %%eax, %%eax    \n"
        "jmp    4f              \n"
        "3:                     \n"
        "sbbl   %%eax, %%eax    \n"
        "orb    $1, %%al        \n"
        "4:                     \n"
        : "=a"(res), "=&S"(d0), "=&D"(d1), "=&c"(d2)
        : "1"(cs), "2"(ct), "3"(count)
        : "memory"
    );

    return res;
}

char *strchr(const char *s, int c)
{
    int d0;
    char *res;

    asm volatile (
        "movb   %%al, %%ah  \n"
        "1:                 \n"
        "lodsb              \n"
        "cmpb   %%ah,%%al   \n"
        "je     2f          \n"
        "testb  %%al,%%al   \n"
        "jne    1b          \n"
        "movl   $1,%1       \n"
        "2:                 \n"
        "movl   %1,%0       \n"
        "decl   %0"
        : "=a"(res), "=&S"(d0)
        : "1"(s), "0"(c)
        : "memory"
    );

    return res;
}

size_t strlen(const char *s)
{
    int d0;
    size_t res;

    asm volatile (
        "repne  \n"
        "scasb  \n"
        : "=c"(res), "=&D"(d0)
        : "1"(s), "a"(0), "0"(0xffffffffu)
        : "memory"
    );

    return ~res - 1;
}

void *memchr(const void *cs, int c, size_t count)
{
    int d0;
    void *res;

    if (!count)
        return NULL;

    asm volatile (
        "repne          \n"
        "scasb          \n"
        "je     1f      \n"
        "movl   $1, %0  \n"
        "1:             \n"
        "decl   %0      \n"
        : "=D"(res), "=&c"(d0)
        : "a"(c), "0"(cs), "1"(count)
        : "memory"
    );

    return res;
}

void *memscan(void *addr, int c, size_t size)
{
    if (!size)
        return addr;

    asm volatile (
        "repnz          \n"
        "scasb          \n"
        "jnz    1f      \n"
        "dec    %%edi   \n"
        "1:             \n"
        : "=D"(addr), "=c"(size)
        : "0"(addr), "1"(size), "a"(c)
        : "memory"
    );

    return addr;
}

size_t strnlen(const char *s, size_t count)
{
    int d0;
    int res;

    asm volatile (
        "movl   %2, %0  \n"
        "jmp    2f      \n"
        "1:             \n"
        "cmpb   $0,(%0) \n"
        "je     3f      \n"
        "incl   %0      \n"
        "2:             \n"
        "decl   %1      \n"
        "cmpl   $-1, %1 \n"
        "jne    1b      \n"
        "3:             \n"
        "subl   %2, %0  \n"
        : "=a"(res), "=&d"(d0)
        : "c"(s), "1"(count)
        : "memory"
    );

    return res;
}
