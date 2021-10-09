/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BITSPERLONG_
#define _ASM_GENERIC_BITSPERLONG_

#ifdef __ARCH_64BIT__
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* ARCH_64BIT */

#endif /* _ASM_GENERIC_BITSPERLONG_ */
