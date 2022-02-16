/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_COMPILER_H_
#define _COMPILER_COMPILER_H_

#ifndef __ASSEMBLY__

#define likely(x)               __builtin_expect(!!(x), 1)
#define unlikely(x)             __builtin_expect(!!(x), 0)
#define likely_notrace(x)       __builtin_expect(!!(x), 1)
#define unlikely_notrace(x)     __builtin_expect(!!(x), 0)

/*
 * Optimization barrier
 * The "volatile" is due to gcc bugs
 */
#ifndef barrier
# define barrier() __asm__ __volatile__("": : :"memory")
#endif

/* Not-quite-unique ID. */
#ifndef __UNIQUE_ID
# define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __LINE__)
#endif

/*
 * Force the compiler to emit 'sym' as a symbol, so that we can reference
 * it from inline assembler. Necessary in case 'sym' could be inlined
 * otherwise, or eliminated entirely due to lack of references that are
 * visible to the compiler.
 */
#define __ADDRESSABLE(sym) \
    static void * __section(".discard.addressable") __used \
    __UNIQUE_ID(__PASTE(__addressable_,sym)) = (void *)&sym;

/**
 * offset_to_ptr - convert a relative memory offset to an absolute pointer
 * @off: the address of the 32-bit offset value
 */
static inline void *offset_to_ptr(unsigned long *off)
{
    return (void *)((unsigned long)off + *off);
}

#include <asm/rwonce.h>

#endif  /* __ASSEMBLY__ */
#endif  /* _COMPILER_H_ */
