/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PROC_H_
#define _ASM_X86_PROC_H_

#include <types.h>
#include <state.h>
#include <asm/page.h>
#include <asm/regs.h>

enum clone_flags;
struct sched_task;
struct task_clone_args;

struct proc_inactive_frame {
#ifdef CONFIG_ARCH_X86_64
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
#else
    uint32_t flags;
    uint32_t si;
    uint32_t di;
#endif

    size_t bx;
    size_t bp;
    size_t ret;
} __packed;

struct proc_frame {
    struct proc_inactive_frame frame;
    struct regs regs;
} __packed;

struct proc_context {
    size_t flags;
    size_t sp;
    size_t fs;
    size_t gs;
};

static __always_inline struct regs *stack_regs(void *stack)
{
    void *stack_start = stack + THREAD_SIZE;
    return ((struct regs *)stack_start) - 1;
}

static __always_inline void cpu_relax(void)
{
    /*
     * rep nop (PAUSE) is a good thing to
     * insert into busy-wait loops.
     */
    asm volatile("rep nop" ::: "memory");
}

extern void proc_thread_setup(struct regs *regs, size_t ip, size_t sp);
extern state proc_thread_copy(struct task_clone_args *args, struct sched_task *child);
extern state proc_thread_switch(struct sched_task *prev);
extern void __noreturn proc_halt(void);
extern void __noreturn proc_reset(void);

#endif /* _ASM_X86_PROC_H_ */
