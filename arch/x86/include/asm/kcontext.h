/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_KCONTEXT_H_
#define _ASM_X86_KCONTEXT_H_

#ifdef CONFIG_ARCH_X86_64
# define KC_BUF_BX    0
# define KC_BUF_BP    1
# define KC_BUF_12    2
# define KC_BUF_13    3
# define KC_BUF_14    4
# define KC_BUF_15    5
# define KC_BUF_SP    6
# define KC_BUF_PC    7
# define KC_BUF_REGS  8
#else
# define KC_BUF_BX    0
# define KC_BUF_SI    1
# define KC_BUF_DI    2
# define KC_BUF_BP    3
# define KC_BUF_SP    4
# define KC_BUF_PC    5
# define KC_BUF_REGS  6
#endif

#include <asm-generic/kcontext.h>

#endif  /* _ASM_X86_KCONTEXT_H_ */
