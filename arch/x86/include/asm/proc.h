/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PROC_H_
#define _ASM_X86_PROC_H_

#include <types.h>
#include <state.h>
#include <asm/regs.h>

struct task;

void proc_thread_setup(struct regs *regs, size_t ip, size_t sp);
state proc_thread_switch(struct task *prev, struct task *next);

void __noreturn proc_reset(void);
void __noreturn proc_halt(void);
void __noreturn proc_poweroff(void);

static __always_inline void cpu_relax(void)
{
    /*
     * rep nop (PAUSE) is a good thing to
     * insert into busy-wait loops.
     */
    asm volatile("rep nop" ::: "memory");
}

#endif /* _ASM_X86_PROC_H_ */
