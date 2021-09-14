/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SCHED_H_
#define _SCHED_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <time.h>
#include <asm/sched.h>

enum task_flags {
    TASK_NONE,
};

struct task {
    struct task *parent;        /* parent of this task */
    struct list_head child;     /* children of this task */
    struct list_head sibling;   /* children list */
    struct task_mm *mm;
    void *stack;

    struct sched_type *sched_type;

    pid_t   pid;
    int8_t  priority;

    struct timespec utime;

#ifdef CONFIG_SMP
    unsigned int cpu_nr;
#endif

    uint64_t    kernel_task:1;
};

struct sched_queue {
    struct task *current;
};

#define SCHED_IDLE_PRIO 127

struct sched_type {
    const char *name;
    struct list_head list;
    char priority;

    struct task *(*task_create)(enum task_flags);
    struct task *(*task_next)(struct sched_queue *);
    void (*task_enqueue)(struct sched_queue *, struct task *);
    void (*task_dequeue)(struct sched_queue *, struct task *);
};

void sched_relax(void);
state sched_register(struct sched_type *sched);
void sched_unregister(struct sched_type *sched);
void sched_init(void);

#endif /* _KERNEL_SCHED_H_ */
