#ifndef _ASM_X86_PROC_H_
#define _ASM_X86_PROC_H_

#include <asm/regs.h>

struct thread_ {

	struct regs regs;	/*  */ 

};

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static __always_inline void cpu_relax(void)
{
	asm volatile("rep; nop" ::: "memory");
}

struct task *arch_switch(struct task *prev, struct task *next);

#endif /* _ASM_X86_PROC_H_ */
