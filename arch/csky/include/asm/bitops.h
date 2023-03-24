/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_BITOPS_H_
#define _ASM_CSKY_BITOPS_H_

#define ffs ffs
static inline unsigned int ffs(unsigned long val)
{
    unsigned int bit;

    if (!val)
        return 0;

    asm volatile (
        "brev   %0\n"
        "ff1    %0\n"
        "addi   %0, 1\n"
        :"=&r"(bit) :"0"(val)
    );

    return bit;
}

#define fls fls
static inline unsigned int fls(unsigned long val)
{
    unsigned int bit;

    asm volatile (
        "ff1    %0\n"
        :"=&r"(bit): "0"(val)
    );

    return (32 - bit) - 1;
}

#include <asm-generic/bitops.h>

#endif /* _ASM_CSKY_BITOPS_H_ */
