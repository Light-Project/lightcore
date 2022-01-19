/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_SETJMP_H_
#define _ASM_GENERIC_SETJMP_H_

typedef struct {
    long regs[JMP_BUF_REGS];
} jmp_buf;

#endif  /* _ASM_GENERIC_SETJMP_H_ */
