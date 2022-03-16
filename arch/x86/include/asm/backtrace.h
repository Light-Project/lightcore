/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_RECALL_H_
#define _ASM_X86_RECALL_H_

#include <asm/regs.h>

extern void dump_stack(size_t *sp);
extern void backtrace(size_t *bp);
extern void show_regs(struct regs *regs, unsigned long error_code);
extern void oops(struct regs *regs, unsigned long error_code, const char *str);

#endif  /* _ASM_X86_RECALL_H_ */
