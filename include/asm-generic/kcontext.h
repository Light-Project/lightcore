/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_KCONTEXT_H_
#define _ASM_GENERIC_KCONTEXT_H_

typedef struct mcontext {
    unsigned long regs[KC_BUF_REGS];
} mcontext_t;

#endif  /* _ASM_GENERIC_CONTEXT_H_ */
