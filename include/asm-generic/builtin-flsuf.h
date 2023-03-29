/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUILTIN_FLSUF_H_
#define _ASM_GENERIC_BUILTIN_FLSUF_H_

#include <bits.h>

#if !__has_builtin(__builtin_clzl)
# error "compiler not supported clzl"
#endif

#define flsuf flsuf
static __always_inline unsigned int flsuf(unsigned long value)
{
	return BITS_PER_LONG - 1 - __builtin_clzl(value);
}

#endif /* _ASM_GENERIC_BUILTIN_FLSUF_H_ */
