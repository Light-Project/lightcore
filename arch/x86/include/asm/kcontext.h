/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_KCONTEXT_H_
#define _ASM_X86_KCONTEXT_H_

#ifdef CONFIG_ARCH_X86_64
# define KC_BUF_BX      0
# define KC_BUF_CX      1
# define KC_BUF_DX      2
# define KC_BUF_SI      3
# define KC_BUF_DI      4
# define KC_BUF_BP      5
# define KC_BUF_8       6
# define KC_BUF_9       7
# define KC_BUF_12      8
# define KC_BUF_13      9
# define KC_BUF_14      10
# define KC_BUF_15      11
# define KC_BUF_SP      12
# define KC_BUF_PC      13
# define KC_BUF_REGS    14
#else
# define KC_BUF_BX      0
# define KC_BUF_SI      1
# define KC_BUF_DI      2
# define KC_BUF_BP      3
# define KC_BUF_SP      4
# define KC_BUF_PC      5
# define KC_BUF_FLAGS   6
# define KC_BUF_REGS    7
#endif

#include <asm-generic/kcontext.h>

#endif  /* _ASM_X86_KCONTEXT_H_ */
