/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PROC_H_
#define _ASM_X86_PROC_H_

#include <types.h>
#include <state.h>

struct task;

static __always_inline void cpu_relax(void)
{
	/*
	 * rep nop (PAUSE) is a good thing to
	 * insert into busy-wait loops.
	 */
	asm volatile("rep nop" ::: "memory");
}

state proc_thread_switch(struct task *prev, struct task *next);

void __noreturn proc_reset(void);
void __noreturn proc_halt(void);

#endif /* _ASM_X86_PROC_H_ */
