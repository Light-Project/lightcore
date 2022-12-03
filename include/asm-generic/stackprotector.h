/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_STACKPROTECTOR_H_
#define _ASM_GENERIC_STACKPROTECTOR_H_

#ifdef CONFIG_64BIT
# ifdef __LITTLE_ENDIAN
#  define CANARY_MASK 0xffffffffffffff00UL
# else /* big endian, 64 bits */
#  define CANARY_MASK 0x00ffffffffffffffUL
# endif
#else /* 32 bits */
# define CANARY_MASK 0xffffffffUL
#endif

#endif  /* _ASM_GENERIC_STACKPROTECTOR_H_ */
