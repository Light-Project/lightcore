/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_STACKPROTECTOR_H_
#define _ASM_X86_STACKPROTECTOR_H_

#include <percpu.h>
#include <prandom.h>
#include <sched.h>

#include <asm/regs.h>
#include <asm-generic/stackprotector.h>

#ifdef CONFIG_STACKPROTECTOR

DECLARE_PERCPU(unsigned long, __stack_chk_guard);

/* NOTE: Keep always inline to prevent stack protection */
static __always_inline void stack_canary_switch(struct sched_task *task)
{
    *thiscpu_ptr(&__stack_chk_guard) = task->stack_canary;
}

#define boot_stack_canary boot_stack_canary
static __always_inline void boot_stack_canary(void)
{
    uint64_t canary;
    uint64_t tsc;

    prandom_bytes(&canary, sizeof(canary));
    tsc = tsc_get();
    canary += tsc + (tsc << 32UL);
    canary &= CANARY_MASK;

    current->stack_canary = canary;
    *thiscpu_ptr(&__stack_chk_guard) = canary;
}

#else

static inline void stack_canary_switch(struct sched_task *task) {}
static inline void boot_stack_canary(void) {}

#endif  /* CONFIG_STACKPROTECTOR */
#endif  /* _ASM_X86_STACKPROTECTOR_H */
