/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUILTIN_FLS_H_
#define _ASM_GENERIC_BUILTIN_FLS_H_

#include <bits.h>

#if !__has_builtin(__builtin_clzl)
# error "compiler not supported clzl"
#endif

#define fls fls
static __always_inline unsigned int fls(unsigned long value)
{
	return value ? BITS_PER_LONG - __builtin_clzl(value) : 0;
}

#endif /* _ASM_GENERIC_BUILTIN_FLS_H_ */
