/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BITOPS_H_
#define _ASM_X86_BITOPS_H_

static __always_inline int ffs(int x)
{
    asm volatile (
        "bsfl   %1,%0\n"
        "jnz    1f\n"
        "movl   $-1,%0\n"
        "1:"
        :"=r"(x) :"rm"(x)
    );
    return x;
}

static __always_inline int fls(unsigned int x)
{
    asm volatile (
        "bsrl   %1,%0\n\t"
        "jnz    1f\n\t"
        "movl   $-1,%0\n"
        "1:"
        :"=r"(x) :"rm"(x)
    );

    return x;
}

#endif  /* _ASM_X86_BITOPS_H_ */
