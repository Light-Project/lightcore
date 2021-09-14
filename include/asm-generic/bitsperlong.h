/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BITS_PER_LONG_
#define _ASM_GENERIC_BITS_PER_LONG_

#ifdef __ARCH_64BIT__
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* ARCH_64BIT */

#endif /* _ASM_GENERIC_BITS_PER_LONG_ */
