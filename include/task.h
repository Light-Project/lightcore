/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TASK_H_
#define _TASK_H_

#include <sched.h>
#include <bits.h>
#include <asm/page.h>

extern unsigned long init_stack[THREAD_SIZE / BYTE_PER_LONG];

struct task_clone_args {
    enum clone_flags flags;
    struct sched_task *curr;
    union {
        struct {
            state (*fn)(void *arg);
            void *arg;
        };
        struct {
            size_t stack;
            size_t stack_size;
        };
    };
};

static inline void *stack_end(void *stack)
{
#ifdef CONFIG_ARCH_STACK_GROWUP
    return stack + THREAD_SIZE - 1;
#else
    return stack;
#endif
}

/* task resource operations */
extern void task_stack_magic(struct sched_task *task);
extern bool task_stack_check(struct sched_task *task);
extern void *task_stack_alloc(void);
extern void task_stack_free(void *stack);
extern struct memory *task_memory_alloc(void);
extern void task_memory_free(struct memory *mem);

extern state fork_thread(enum clone_flags flags, int (*fn)(void *), void *arg);
extern void __init fork_init(void);

#endif  /* _TASK_H_ */
