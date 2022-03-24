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
#include <fstype.h>
#include <static-call.h>
#include <kcontext.h>
#include <lightcore/sched.h>

#define TASK_NAME_LEN   32
struct namespace;

enum sched_preempt {
    SCHED_PREEMPT_UNINIT    = 0,
    SCHED_PREEMPT_NONE      = 1,
    SCHED_PREEMPT_VOLUNTARY = 2,
    SCHED_PREEMPT_FULL      = 3,
    SCHED_PREEMPT_NR_MAX,
};

enum sched_prio {
    SCHED_PRIO_DL           = 10,
    SCHED_PRIO_FIFO         = 20,
    SCHED_PRIO_RR           = 25,
    SCHED_PRIO_FAIR         = 30,
    SCHED_PRIO_IDLE         = 31,
    SCHED_PRIO_MAX,
};

enum sched_task_state {
    SCHED_TASK_RUNNING      = 0x0000,
    SCHED_TASK_NEW          = 0x0001,
};

enum sched_task_flags {
    __SCHED_TASK_KTHREAD,
    __SCHED_TASK_IDLE,
    __SCHED_TASK_NRESCHED,
};

#define SCHED_TASK_KTHREAD      BIT(__SCHED_TASK_KTHREAD)
#define SCHED_TASK_IDLE         BIT(__SCHED_TASK_IDLE)
#define SCHED_TASK_NRESCHED     BIT(__SCHED_TASK_NRESCHED)

enum sched_task_queued {
    __SCHED_TASK_QUEUED,
    __SCHED_TASK_MIGRATING,
};

#define SCHED_TASK_QUEUED       BIT(__SCHED_TASK_QUEUED)
#define SCHED_TASK_MIGRATING    BIT(__SCHED_TASK_MIGRATING)

enum sched_queue_flags {
    __SCHED_QUEUE_SAVE,
    __SCHED_QUEUE_MOVE,
    __SCHED_QUEUE_NOCLOCK,
    __SCHED_ENQUEUE_HEAD,
    __SCHED_ENQUEUE_WEAKUP,
    __SCHED_DEQUEUE_SLEEP,
};

#define SCHED_QUEUE_SAVE        BIT(__SCHED_QUEUE_SAVE)
#define SCHED_QUEUE_MOVE        BIT(__SCHED_QUEUE_MOVE)
#define SCHED_QUEUE_NOCLOCK     BIT(__SCHED_QUEUE_NOCLOCK)
#define SCHED_ENQUEUE_HEAD      BIT(__SCHED_ENQUEUE_HEAD)
#define SCHED_ENQUEUE_WEAKUP    BIT(__SCHED_ENQUEUE_WEAKUP)
#define SCHED_DEQUEUE_SLEEP     BIT(__SCHED_DEQUEUE_SLEEP)

/**
 * sched_task - basic task structure
 * @sibling: list node of @child
 * @child: children of this task
 * @parent: parent of this task
 */
struct sched_task {
    struct list_head sibling;
    struct list_head child;
    struct sched_task *parent;
    struct namespace *namespace;
    char name[TASK_NAME_LEN];

    /* context informations */
    struct kcontext kcontext;
    struct proc_context proc;
    struct memory *active_mem;
    struct memory *mem;
    void *stack;

    /* scheduler informations */
    struct sched_type *sched_type;
    struct pid *pid_struct;
    char priority;

    pid_t pid;
    pid_t tgid;

    uint64_t start_time;
    struct timespec utime;
    uint64_t nivcsw;
    uint64_t nvcsw;

    unsigned long state;
    unsigned long flags;
    unsigned long queued;

#ifdef CONFIG_SMP
    struct sched_queue *queue;
#endif

    struct filesystem *fs;
    void *pdata;
};

#define kcontext_to_task(ptr) \
    container_of_safe(ptr, struct sched_task, kcontext)

/**
 * sched_queue - basic queue structure
 * @nr_running: total tasks number on this queue
 * @context_switches: context switches number occurred on this queue
 * @curr: current task running on this queue
 * @idle: idle task pointer
 * @prev_mem:
 * @pdata: private data of scheduler
 */
struct sched_queue {
    struct list_head tasks;
    unsigned int nr_running;
    uint64_t context_switches;

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
    void (*task_yield_to)(struct sched_queue *queue, struct sched_task *task);
    struct sched_task *(*task_pick)(struct sched_queue *queue);

#ifdef CONFIG_SMP
    void (*queue_online)(struct sched_queue *queue);
    void (*queue_offline)(struct sched_queue *queue);
#endif
};

extern struct sched_task init_task;
extern struct sched_type idle_sched;
extern struct sched_type *default_sched;

extern enum sched_preempt sched_preempt_mode;
extern const char *sched_preempt_name[SCHED_PREEMPT_NR_MAX];

extern struct list_head sched_list;
DECLARE_PERCPU(struct sched_queue, sched_queues);
DECLARE_PERCPU(unsigned long, sched_preempt_count);

extern struct kcache *task_cache;
extern const unsigned int sched_prio_to_weight[40];
extern const unsigned int sched_prio_to_wmult[40];

#define preempt_count sched_get_preempt()
static inline unsigned long sched_get_preempt(void)
{
    return thiscpu(sched_preempt_count);
}

#define current_queue sched_current_queue()
static inline struct sched_queue *sched_current_queue(void)
{
    return thiscpu_ptr(&sched_queues);
}

#ifndef current
#define current sched_current_task()
static inline struct sched_task *sched_current_task(void)
{
    return READ_ONCE(current_queue->curr);
}
#endif

#ifndef current_set
#define current_set(task) sched_current_task_set(task)
static inline void sched_current_task_set(struct sched_task *task)
{
    WRITE_ONCE(current_queue->curr, task);
}
#endif

GENERIC_STRUCT_BITOPS(task, struct sched_task, flags)
#define task_flags_set(task, bit)   generic_task_flags_set(task, bit)
#define task_flags_clr(task, bit)   generic_task_flags_clr(task, bit)
#define task_flags_test(task, bit)  generic_task_flags_test(task, bit)

GENERIC_STRUCT_BITOPS(task, struct sched_task, queued);
#define task_queued_set(task, bit)  generic_task_queued_set(task, bit)
#define task_queued_clr(task, bit)  generic_task_queued_clr(task, bit)
#define task_queued_test(task, bit) generic_task_queued_test(task, bit)

#define task_set_resched(task)      task_flags_set(task, __SCHED_TASK_NRESCHED)
#define task_clr_resched(task)      task_flags_clr(task, __SCHED_TASK_NRESCHED)
#define task_need_resched(task)     task_flags_test(task, __SCHED_TASK_NRESCHED)

#define queue_set_resched(queue)    task_set_resched(queue->curr)
#define queue_clr_resched(queue)    task_clr_resched(queue->curr)
#define queue_need_resched(queue)   task_need_resched(queue->curr)

#define current_set_resched()       task_set_resched(current)
#define current_clr_resched()       task_clr_resched(current)
#define current_need_resched()      task_need_resched(current)

extern bool sched_cond_resched_handle(void);
extern void sched_preempt_handle(void);

DECLARE_STATIC_CALL(sched_cond_resched, sched_cond_resched_handle);
DECLARE_STATIC_CALL(sched_might_resched, sched_cond_resched_handle);
DECLARE_STATIC_CALL(sched_preempt, sched_preempt_handle);

#define sched_cond_resched()    static_call_cond(sched_cond_resched)()
#define sched_might_resched()   static_call_cond(sched_might_resched)()
#define sched_preempt()         static_call_cond(sched_preempt)()

/* idle task */
extern void __noreturn idle_task_entry(void);

/* scheduler operation */
extern void scheduler_resched(void);
extern void scheduler_kill(void);
extern void scheduler_tick(void);
extern void scheduler_idle(void);
extern void scheduler_yield(void);

/* scheduler task */
extern void sched_task_wake_up(struct sched_task *task);
extern state sched_task_clone(struct sched_task *task, enum clone_flags flags);
extern struct sched_task *sched_task_create(void);
extern void sched_task_destroy(struct sched_task *task);

/* scheduler queue */
extern struct sched_type *sched_find(const char *name, unsigned char prio);
extern state sched_register(struct sched_type *sched);
extern void sched_unregister(struct sched_type *sched);
extern void __init sched_init(void);

#endif /* _KERNEL_SCHED_H_ */
