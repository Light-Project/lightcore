/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_RECALL_H_
#define _ASM_X86_RECALL_H_

#include <asm/regs.h>

extern void show_regs(struct regs *regs);
extern void dump_stack(size_t *sp);
extern void backtrace(size_t *bp);
extern void oops(struct regs *regs, const char *str);

#endif  /* _ASM_X86_RECALL_H_ */
