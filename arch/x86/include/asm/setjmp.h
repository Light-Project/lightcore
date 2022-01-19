/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_SETJMP_H_
#define _ASM_X86_SETJMP_H_

#ifdef CONFIG_ARCH_X86_64
# define JMP_BUF_BX     0
# define JMP_BUF_BP     1
# define JMP_BUF_12     2
# define JMP_BUF_13     3
# define JMP_BUF_14     4
# define JMP_BUF_15     5
# define JMP_BUF_SP     6
# define JMP_BUF_PC	    7
# define JMP_BUF_REGS   8
#else
# define JMP_BUF_BX     0
# define JMP_BUF_SI     1
# define JMP_BUF_DI     2
# define JMP_BUF_BP     3
# define JMP_BUF_SP     4
# define JMP_BUF_PC     5
# define JMP_BUF_REGS   6
#endif

#include <asm-generic/setjmp.h>

#endif  /* _ASM_X86_SETJMP_H_ */
