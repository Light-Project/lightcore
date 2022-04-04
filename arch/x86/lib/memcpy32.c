/* SPDX-License-Identifier: GPL-2.0 */
#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
    int d0, d1, res;

    asm volatile (
        "1:                     \n"
        "lodsb                  \n"
        "scasb                  \n"
        "jne    2f              \n"
        "loop   1b              \n"
        "xorl   %%eax, %%eax    \n"
        "jmp    3f              \n"
        "2:                     \n"
        "sbbl   %%eax, %%eax    \n"
        "orb    $1, %%al        \n"
        "3:                     \n"
        : "=a"(res), "=&S"(d0), "=&D"(d1)
        : "1"(s1), "2"(s2), "c"(n)
        : "memory"
    );

    return res;
}

void *memset(void *s, int c, size_t count)
{
    int d0, d1;

    asm volatile (
        "rep    \n"
        "stosb  \n"
        : "=&c"(d0), "=&D"(d1)
        : "a"(c), "1"(s), "0"(count)
        : "memory"
    );

    return s;
}

void *memcpy(void *to, const void *from, size_t n)
{
    int d0, d1, d2;

    asm volatile (
        "rep                \n"
        "movsl              \n"
        "movl   %4,%%ecx    \n"
        "andl   $3,%%ecx    \n"
        "jz     1f          \n"
        "rep                \n"
        "movsb              \n"
        "1:                 \n"
        : "=&c"(d0), "=&D"(d1), "=&S"(d2)
        : "0"(n / 4), "g"(n), "1"((long)to), "2"((long)from)
        : "memory"
    );

    return to;
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

void *memmove(void *dest, const void *src, size_t n)
{
    int d0, d1, d2, d3, d4, d5;
    char *ret = dest;

    asm volatile (
        /* Handle more 16 bytes in loop */
        "cmp    $0x10, %0           \n"
        "jb     1f                  \n"

        /* Decide forward/backward copy mode */
        "cmp    %2, %1              \n"
        "jb     2f                  \n"

        /*
         * movs instruction have many startup latency
         * so we handle small size by general register.
         */
        "cmp    $680, %0            \n"
        "jb     3f                  \n"

        /*
         * movs instruction is only good for aligned case.
         */
        "mov    %1, %3              \n"
        "xor    %2, %3              \n"
        "and    $0xff, %3           \n"
        "jz     4f                  \n"
        "3:                         \n"
        "sub    $0x10, %0           \n"

        /*
         * We gobble 16 bytes forward in each loop.
         */
        "3:                         \n"
        "sub    $0x10, %0           \n"
        "mov    0*4(%1), %3         \n"
        "mov    1*4(%1), %4         \n"
        "mov    %3, 0*4(%2)         \n"
        "mov    %4, 1*4(%2)         \n"
        "mov    2*4(%1), %3         \n"
        "mov    3*4(%1), %4         \n"
        "mov    %3, 2*4(%2)         \n"
        "mov    %4, 3*4(%2)         \n"
        "lea    0x10(%1), %1        \n"
        "lea    0x10(%2), %2        \n"
        "jae    3b                  \n"
        "add    $0x10, %0           \n"
        "jmp    1f                  \n"

        /*
         * Handle data forward by movs.
         */
        ".p2align 4                 \n"
        "4:                         \n"
        "mov    -4(%1, %0), %3      \n"
        "lea    -4(%2, %0), %4      \n"
        "shr    $2, %0              \n"
        "rep    movsl               \n"
        "mov    %3, (%4)            \n"
        "jmp    11f                 \n"

        /*
         * Handle data backward by movs.
         */
        ".p2align 4                 \n"
        "6:                         \n"
        "mov    (%1), %3            \n"
        "mov    %2, %4              \n"
        "lea    -4(%1, %0), %1      \n"
        "lea    -4(%2, %0), %2      \n"
        "shr    $2, %0              \n"
        "std                        \n"
        "rep    movsl               \n"
        "mov    %3,(%4)             \n"
        "cld                        \n"
        "jmp    11f                 \n"

        /*
         * Start to prepare for backward copy.
         */
        ".p2align 4                 \n"
        "2:                         \n"
        "cmp    $680, %0            \n"
        "jb     5f                  \n"
        "mov    %1, %3              \n"
        "xor    %2, %3              \n"
        "and    $0xff, %3           \n"
        "jz     6b                  \n"

        /*
         * Calculate copy position to tail.
         */
        "5:                         \n"
        "add    %0, %1              \n"
        "add    %0, %2              \n"
        "sub    $0x10, %0           \n"

        /*
         * We gobble 16 bytes backward in each loop.
         */
        "7:                         \n"
        "sub    $0x10, %0           \n"

        "mov    -1*4(%1), %3        \n"
        "mov    -2*4(%1), %4        \n"
        "mov    %3, -1*4(%2)        \n"
        "mov    %4, -2*4(%2)        \n"
        "mov    -3*4(%1), %3        \n"
        "mov    -4*4(%1), %4        \n"
        "mov    %3, -3*4(%2)        \n"
        "mov    %4, -4*4(%2)        \n"
        "lea    -0x10(%1), %1       \n"
        "lea    -0x10(%2), %2       \n"
        "jae    7b                  \n"

        /*
         * Calculate copy position to head.
         */
        "add    $0x10, %0           \n"
        "sub    %0, %1              \n"
        "sub    %0, %2              \n"

        /*
         * Move data from 8 bytes to 15 bytes.
         */
        ".p2align 4                 \n"
        "1:                         \n"
        "cmp    $8, %0              \n"
        "jb     8f                  \n"
        "mov    0*4(%1), %3         \n"
        "mov    1*4(%1), %4         \n"
        "mov    -2*4(%1, %0), %5    \n"
        "mov    -1*4(%1, %0), %1    \n"

        "mov    %3, 0*4(%2)         \n"
        "mov    %4, 1*4(%2)         \n"
        "mov    %5, -2*4(%2, %0)    \n"
        "mov    %1, -1*4(%2, %0)    \n"
        "jmp    11f                 \n"

        /*
         * Move data from 4 bytes to 7 bytes.
         */
        ".p2align 4                 \n"
        "8:                         \n"
        "cmp    $4, %0              \n"
        "jb     9f                  \n"
        "mov    0*4(%1), %3         \n"
        "mov    -1*4(%1, %0), %4    \n"
        "mov    %3, 0*4(%2)         \n"
        "mov    %4, -1*4(%2, %0)    \n"
        "jmp    11f                 \n"

        /*
         * Move data from 2 bytes to 3 bytes.
         */
        ".p2align 4                 \n"
        "9:                         \n"
        "cmp    $2, %0              \n"
        "jb     10f                 \n"
        "movw   0*2(%1), %%dx       \n"
        "movw   -1*2(%1, %0), %%bx  \n"
        "movw   %%dx, 0*2(%2)       \n"
        "movw   %%bx, -1*2(%2, %0)  \n"
        "jmp    11f                 \n"

        /*
         * Move data for 1 byte.
         */
        ".p2align 4                 \n"
        "10:                        \n"
        "cmp    $1, %0              \n"
        "jb     11f                 \n"
        "movb   (%1), %%cl          \n"
        "movb   %%cl, (%2)          \n"
        ".p2align 4                 \n"
        "11:                        \n"
        : "=&c" (d0), "=&S" (d1), "=&D" (d2),
          "=r" (d3),"=r" (d4), "=r"(d5)
        : "0" (n),
          "1" (src),
          "2" (dest)
        : "memory"
    );

    return ret;
}
