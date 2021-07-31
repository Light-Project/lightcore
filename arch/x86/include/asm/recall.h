/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_RECALL_H_ 
#define _ASM_X86_RECALL_H_

#include <asm/regs.h>

void recall(const char *str, struct regs *regs);

#endif  /* _ASM_X86_RECALL_H_ */
