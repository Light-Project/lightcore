/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_RECALL_H_
#define _ASM_X86_RECALL_H_

#include <asm/regs.h>

void backtrace_regs(struct regs *regs);
void backtrace_stack(size_t *sp);
void backtrace(struct regs *regs);

#endif  /* _ASM_X86_RECALL_H_ */
