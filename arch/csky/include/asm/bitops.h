/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_BITOPS_H_
#define _ASM_CSKY_BITOPS_H_

static inline int ffs(int x)
{
    if (!x)
        return 0;

    asm volatile (
        "brev   %0\n"
        "ff1    %0\n"
        "addi   %0, 1\n"
        :"=&r"(x) :"0"(x)
    );

    return x;
}

static inline int fls(unsigned int x)
{
    asm volatile (
        "ff1    %0\n"
        :"=&r"(x): "0"(x)
    );

    return (32 - x);
}

#endif  /* _ASM_CSKY_BITOPS_H_ */
