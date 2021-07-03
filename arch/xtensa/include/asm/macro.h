#ifndef _ASM_XTENSA_MACRO_H_
#define _ASM_XTENSA_MACRO_H_

#include <asm/core.h>
#include <types.h>

#if XCHAL_HAVE_MINMAX
#define arch_min arch_min
static inline size_t arch_min(size_t a, size_t b)
{
    size_t ret;
    asm volatile(
        "min    %2, %0, %1\n\r"
        :"=a"(ret):"a"(a),"a"(b):"memory"
    );
    return ret;
}

#define arch_max arch_max
static inline size_t arch_max(size_t a, size_t b)
{
    size_t ret;
    asm volatile(
        "max    %2, %0, %1\n\r"
        :"=a"(ret):"a"(a),"a"(b):"memory"
    );
    return ret;
}
#endif /* XCHAL_HAVE_MINMAX */

#endif /* _ASM_XTENSA_MACRO_H_ */
