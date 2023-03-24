/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TASK_H_
#define _TASK_H_

#include <sched.h>
#include <asm/page.h>

extern unsigned long init_stack[THREAD_SIZE / BYTES_PER_LONG];

struct task_clone_args {
    enum clone_flags flags;
    unsigned int exit_signal;
    pid_t *tid;
    size_t tid_size;
    union {
        struct {
            state (*entry)(void *arg);
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

/* Task stack operations */
extern void task_stack_magic(struct sched_task *task);
extern bool task_stack_check(struct sched_task *task);
extern void *task_stack_alloc(void);
extern void task_stack_free(void *stack);

/* Task memory struct operations */
extern struct memory *task_memory_alloc(void);
extern void task_memory_free(struct memory *mem);

/* Kernel task operations */
extern state kernel_clone(enum clone_flags flags, int (*fn)(void *), void *arg);
extern state kernel_wait(pid_t pid, state *exit_code);
extern void __noreturn kernel_exit(state exit_code);

extern void __init clone_init(void);

#endif /* _TASK_H_ */
