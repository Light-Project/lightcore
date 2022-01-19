/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SCHED_H_
#define _SCHED_H_

#include <bits.h>
#include <list.h>
#include <pid.h>
#include <numa.h>
#include <time.h>
#include <proc.h>
#include <percpu.h>
#include <lightcore/sched.h>

#define TASK_NAME_LEN   32
struct namespace;

enum sched_prio {
    SCHED_PRIO_DL       = 10,
    SCHED_PRIO_FIFO     = 20,
    SCHED_PRIO_RR       = 25,
    SCHED_PRIO_FAIR     = 30,
    SCHED_PRIO_IDLE     = 31,
    SCHED_PRIO_MAX      = 32,
};

enum sched_task_state {
    SCHED_TASK_NEW          = BIT(0),
};

enum sched_task_flags {
    SCHED_TASK_KTHREAD      = BIT(0),
    SCHED_TASK_IDLE         = BIT(1),
};

/**
 * enum sched_queue_flags -
 * @head:
 * @weakup:
 */
enum sched_queue_flags {
    SCHED_QUEUE_SAVE        = BIT(0),
    SCHED_QUEUE_MOVE        = BIT(1),
    SCHED_ENQUEUE_HEAD      = BIT(8),
    SCHED_ENQUEUE_WEAKUP    = BIT(9),
    SCHED_DEQUEUE_SLEEP     = BIT(16),
};

/**
 * sched_task - basic task structure
 *
 */
struct sched_task {
    struct list_head sibling;   /* children list */
    struct list_head child;     /* children of this task */
    struct sched_task *parent;  /* parent of this task */
    struct namespace *ns;
    char name[TASK_NAME_LEN];

    /* context informations */
    struct proc_context proc;
    struct memory *active_mem;
    struct memory *mem;
    void *stack;

    /* scheduler informations */
    struct sched_type *sched_type;
    struct timespec utime;
    struct pid pid;
    char priority;

    enum sched_task_state state;
    enum sched_task_flags flags;

#ifdef CONFIG_SMP
    struct sched_queue *queue;
#endif

    void *pdata;
};

/**
 * sched_queue - basic queue structure
 *
 */
struct sched_queue {
    struct list_head run_list;
    unsigned int nr_running;

    struct sched_task *curr;
    struct sched_task *idle;
    struct memory *prev_mem;

    void *pdata[SCHED_PRIO_MAX];
};

/**
 * sched_type - describes a scheduler
 * @priority: scheduler priority
 * @queue_create: create a scheduler private queue
 * @queue_destroy: destroy a scheduler private queue
 * @task_enqueue: enqueue a task into scheduler private queue
 * @task_dequeue: dequeue a task out of scheduler private queue
 * @task_create: create a private task that contains the base task
 * @task_destroy: destroy a private task
 * @task_tick: tick event for the current task
 * @task_prio: reset task priority
 * @task_prev: put back the previous task
 * @task_next: set the next task to fetch
 * @task_yield: yield the current task and switch to next task
 * @task_pick: get highest priority task on this scheduler queue
 */
struct sched_type {
    struct list_head list;
    const char *name;
    unsigned char priority;

    /* scheduler queue operations */
    state (*queue_create)(struct sched_queue *queue);
    void (*queue_destroy)(struct sched_queue *queue);
    void (*task_enqueue)(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags);
    void (*task_dequeue)(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags);

    /* scheduler task operations */
    struct sched_task *(*task_create)(int numa);
    struct sched_task *(*task_fork)(struct sched_queue *queue);
    void (*task_destroy)(struct sched_task *task);
    void (*task_tick)(struct sched_queue *queue, struct sched_task *task);
    void (*task_prio)(struct sched_queue *queue, struct sched_task *task, unsigned int prio);
    void (*task_prev)(struct sched_queue *queue, struct sched_task *task);
    void (*task_next)(struct sched_queue *queue, struct sched_task *task);
    void (*task_yield)(struct sched_queue *queue);
    struct sched_task *(*task_pick)(struct sched_queue *queue);

#ifdef CONFIG_SMP
    void (*queue_online)(struct sched_queue *queue);
    void (*queue_offline)(struct sched_queue *queue);
#endif
};

extern struct sched_type *default_sched;
extern struct list_head sched_list;
extern struct kcache *task_cache;
DECLARE_PERCPU(struct sched_queue, sched_queues);

/* idle task */
extern void __noreturn idle_task_entry(void);

/* sched core */
extern struct sched_task *sched_current(void);
#define current sched_current()

extern void sched_relax(void);
extern void sched_tick(void);
extern void sched_task_fork(struct sched_task *child);
extern struct sched_task *sched_task_create(void);
extern void sched_task_destroy(struct sched_task *task);
extern void __init sched_cache_init(void);

/* scheduler queue */
extern struct sched_type *sched_find(const char *name, unsigned char prio);
extern state sched_register(struct sched_type *sched);
extern void sched_unregister(struct sched_type *sched);
extern void __init sched_init(void);

#endif /* _KERNEL_SCHED_H_ */
