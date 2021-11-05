/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SCHED_H_
#define _SCHED_H_

#include <types.h>
#include <state.h>
#include <error.h>
#include <bits.h>
#include <list.h>
#include <pid.h>
#include <time.h>

struct namespace;

enum sched_prio {
    SCHED_PRIO_DEADLINE = 5,
    SCHED_PRIO_RR       = 15,
    SCHED_PRIO_FAIR     = 20,
    SCHED_PRIO_IDLE     = 31,
    SCHED_PRIO_MAX,
};

struct task {
    struct task *parent;        /* parent of this task */
    struct list_head child;     /* children of this task */
    struct list_head sibling;   /* children list */

    struct sched_type *sched_type;
    struct vmem_area *vmem;
    void *stack;

    struct namespace *ns;
    struct pid pid;
    char priority;

    struct timespec utime;

#ifdef CONFIG_SMP
    struct sched_queue *queue;
#endif

    uint64_t    idle:1;
    uint64_t    kthread:1;
};

struct sched_queue {
    struct list_head list;
    struct task *curr;

    void *priv[SCHED_PRIO_MAX];
};

struct sched_type {
    const char *name;
    struct list_head list;
    unsigned char priority;

    state (*queue_create)(struct sched_queue *);
    void (*queue_destroy)(struct sched_queue *);

    struct task *(*task_create)(int flags);
    void (*task_destroy)(struct task *);

    void (*task_enqueue)(struct sched_queue *, struct task *, int flags);
    void (*task_dequeue)(struct sched_queue *, struct task *, int flags);

    void (*task_tick)(struct sched_queue *, struct task *);
    void (*task_next)(struct sched_queue *, struct task *);
    void (*task_prev)(struct sched_queue *, struct task *);
    struct task *(*task_pick)(struct sched_queue *);

#ifdef CONFIG_SMP

#endif
};

extern struct sched_type *default_sched;

extern void sched_relax(void);
extern void sched_tick(void);

extern state sched_task_start(struct task *task);
struct task *sched_task_create(const char *schedn, int flags);
extern void sched_task_destroy(struct task *task);

extern state sched_register(struct sched_type *sched);
extern void sched_unregister(struct sched_type *sched);
extern void __init sched_init(void);
extern void __init fork_init(void);

#endif /* _KERNEL_SCHED_H_ */
