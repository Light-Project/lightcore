/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_SCHED_H_
#define _KERNEL_SCHED_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <time.h>

struct task {
    struct task *parent;        /* parent of this task */
    struct list_head child;     /* children of this task */
    struct list_head sibling;   /* children list */
    struct task_mm *mm;
    struct regs *stack;

    pid_t   pid;
    uint8_t prio;

    struct timespec utime;

#ifdef CONFIG_SMP
    unsigned int cpu_nr;
#endif

    uint64_t    kernel_task:1;
};

struct sched_queue {

};

struct sched_type {
    const char *name;
    struct list_head list;
    unsigned char priority;

    void (*task_setup)(struct task *);
    void (*task_enqueue)(struct sched_queue *, struct task *);
    void (*task_dequeue)(struct sched_queue *, struct task *);
};

void sched_relax(void);
void sched_init(void);

state sched_register(struct sched_type *sched);
void sched_unregister(struct sched_type *sched);

#endif /* _KERNEL_SCHED_H_ */
