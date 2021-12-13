/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_TRAPS_H_
#define _ASM_X86_TRAPS_H_

#include <asm/regs.h>
#include <asm/segment.h>

static inline bool trap_v8086_mode(struct regs *regs)
{
#ifdef CONFIG_ARCH_X86_64
    return false;
#else
    return !!(regs->flags & EFLAGS_VM);
#endif
}

static inline bool trap_user_mode(struct regs *regs)
{
#ifdef CONFIG_ARCH_X86_64
    return !!(regs->cs & 0x03);
#else
    return ((regs->cs & 0x03) | (regs->flags & EFLAGS_VM)) >= USER_RPL;
#endif
}

#endif /* _ASM_X86_TRAPS_H_ */
