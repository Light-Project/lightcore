/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_ASM_GENERIC_BITPERLONG_H_
#define _LIGHTCORE_ASM_GENERIC_BITPERLONG_H_

#ifndef BITS_PER_LONG
# define BITS_PER_LONG (__SIZEOF_POINTER__ * 8)
#endif

#define small_const_nbits(nbits) \
    (__builtin_constant_p(nbits) && \
    (nbits) <= BITS_PER_LONG && (nbits) > 0)

#endif  /* _LIGHTCORE_ASM_GENERIC_BITPERLONG_H_ */
