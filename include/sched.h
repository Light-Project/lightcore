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
#include <preempt.h>
#include <fstype.h>
#include <refcount.h>
#include <bitflags.h>
#include <wakequeue.h>
#include <static-call.h>
#include <kcontext.h>
#include <lightcore/sched.h>

#define TASK_NAME_LEN   32
struct namespace;

enum sched_preempt {
    SCHED_PREEMPT_UNINIT        = 0,
    SCHED_PREEMPT_NONE          = 1,
    SCHED_PREEMPT_VOLUNTARY     = 2,
    SCHED_PREEMPT_FULL          = 3,
    SCHED_PREEMPT_NR_MAX,
};

enum sched_prio {
    SCHED_PRIO_DL               = 10,
    SCHED_PRIO_FIFO             = 20,
    SCHED_PRIO_RR               = 25,
    SCHED_PRIO_FAIR             = 30,
    SCHED_PRIO_IDLE             = 31,
    SCHED_PRIO_MAX,
};

enum sched_task_state {
    __SCHED_TASK_INTERRUPTIBLE,
    __SCHED_TASK_UNINTERRUPTIBLE,
    __SCHED_TASK_NEW,
    __SCHED_TASK_STOPPED,
    __SCHED_TASK_KILL,
};

#define SCHED_TASK_RUNNING          0
#define SCHED_TASK_INTERRUPTIBLE    BIT(__SCHED_TASK_INTERRUPTIBLE)
#define SCHED_TASK_UNINTERRUPTIBLE  BIT(__SCHED_TASK_UNINTERRUPTIBLE)
#define SCHED_TASK_NEW              BIT(__SCHED_TASK_NEW)
#define SCHED_TASK_STOPPED          BIT(__SCHED_TASK_STOPPED)
#define SCHED_TASK_KILL             BIT(__SCHED_TASK_KILL)
#define SCHED_TASK_BLOCKED (SCHED_TASK_INTERRUPTIBLE | SCHED_TASK_UNINTERRUPTIBLE)

enum sched_task_exit {
    __SCHED_TASK_DEAD,
    __SCHED_TASK_ZOMBIE,
};

#define SCHED_TASK_DEAD             BIT(__SCHED_TASK_DEAD)
#define SCHED_TASK_ZOMBIE           BIT(__SCHED_TASK_ZOMBIE)

enum sched_task_flags {
    __SCHED_TASK_KTHREAD,
    __SCHED_TASK_KCORO,
    __SCHED_TASK_IDLE,
    __SCHED_TASK_NRESCHED,
    __SCHED_TASK_SIGNAL,
};

#define SCHED_TASK_KTHREAD          BIT(__SCHED_TASK_KTHREAD)
#define SCHED_TASK_KCORO            BIT(__SCHED_TASK_KCORO)
#define SCHED_TASK_IDLE             BIT(__SCHED_TASK_IDLE)
#define SCHED_TASK_NRESCHED         BIT(__SCHED_TASK_NRESCHED)
#define SCHED_TASK_SIGNAL           BIT(__SCHED_TASK_SIGNAL)

enum sched_task_queued {
    __SCHED_TASK_QUEUED,
    __SCHED_TASK_MIGRATING,
};

#define SCHED_TASK_QUEUED           BIT(__SCHED_TASK_QUEUED)
#define SCHED_TASK_MIGRATING        BIT(__SCHED_TASK_MIGRATING)

enum sched_queue_flags {
    __SCHED_QUEUE_SAVE,
    __SCHED_QUEUE_MOVE,
    __SCHED_QUEUE_NOCLOCK,
    __SCHED_ENQUEUE_HEAD,
    __SCHED_ENQUEUE_WEAKUP,
    __SCHED_DEQUEUE_SLEEP,
};

#define SCHED_QUEUE_SAVE            BIT(__SCHED_QUEUE_SAVE)
#define SCHED_QUEUE_MOVE            BIT(__SCHED_QUEUE_MOVE)
#define SCHED_QUEUE_NOCLOCK         BIT(__SCHED_QUEUE_NOCLOCK)
#define SCHED_ENQUEUE_HEAD          BIT(__SCHED_ENQUEUE_HEAD)
#define SCHED_ENQUEUE_WEAKUP        BIT(__SCHED_ENQUEUE_WEAKUP)
#define SCHED_DEQUEUE_SLEEP         BIT(__SCHED_DEQUEUE_SLEEP)

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

    struct list_head ptraced;
    struct list_head ptrace_sibling;
    struct wake_queue wakeq;

    /* context informations */
    struct kcontext kcontext;
    struct proc_context proc;
    struct memory *active_mem;
    struct memory *mem;
    refcount_t refcount;
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
    unsigned long exit;
    unsigned long flags;
    unsigned long queued;

    state errnum;
    state exit_code;

    struct filesystem *fs;
    void *pdata;

#ifdef CONFIG_SMP
    struct sched_queue *queue;
#endif

#ifdef CONFIG_UBSAN
    unsigned int ubsan_depth;
#endif

#ifdef CONFIG_STACKPROTECTOR
    unsigned long stack_canary;
#endif
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

extern struct kcache *task_cache;
extern const unsigned int sched_prio_to_weight[40];
extern const unsigned int sched_prio_to_wmult[40];

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

#ifndef current_get_state
static inline unsigned long current_get_state(void)
{
    return READ_ONCE(current->state);
}
#endif

#ifndef current_set_state
static inline void current_set_state(unsigned long state)
{
    WRITE_ONCE(current->state, state);
}
#endif

static inline unsigned long task_get_state(struct sched_task *task)
{
    return READ_ONCE(task->state);
}

static inline void task_set_state(struct sched_task *task, unsigned long state)
{
    WRITE_ONCE(task->state, state);
}

GENERIC_STRUCT_BITOPS(task, struct sched_task, flags, false)
GENERIC_STRUCT_BITOPS(task, struct sched_task, queued, false)

#define SCHED_TASK_FLAG_OPS(ops, name, bit)                                     \
static inline void task_set_##name(struct sched_task *task)                     \
{                                                                               \
    ops##_set(task, bit);                                                       \
}                                                                               \
                                                                                \
static inline void task_clr_##name(struct sched_task *task)                     \
{                                                                               \
    ops##_clr(task, bit);                                                       \
}                                                                               \
                                                                                \
static inline bool task_test_##name(struct sched_task *task)                    \
{                                                                               \
    return ops##_test(task, bit);                                               \
}                                                                               \
                                                                                \
static inline void queue_set_##name(struct sched_queue *queue)                  \
{                                                                               \
    task_set_##name(queue->curr);                                               \
}                                                                               \
                                                                                \
static inline void queue_clr_##name(struct sched_queue *queue)                  \
{                                                                               \
    task_clr_##name(queue->curr);                                               \
}                                                                               \
                                                                                \
static inline bool queue_test_##name(struct sched_queue *queue)                 \
{                                                                               \
    return task_test_##name(queue->curr);                                       \
}                                                                               \
                                                                                \
static inline void current_set_##name(void)                                     \
{                                                                               \
    task_set_##name(current);                                                   \
}                                                                               \
                                                                                \
static inline void current_clr_##name(void)                                     \
{                                                                               \
    task_clr_##name(current);                                                   \
}                                                                               \
                                                                                \
static inline bool current_test_##name(void)                                    \
{                                                                               \
    return task_test_##name(current);                                           \
}

SCHED_TASK_FLAG_OPS(task_flags, kthread, __SCHED_TASK_KTHREAD)
SCHED_TASK_FLAG_OPS(task_flags, kcoro, __SCHED_TASK_KCORO)
SCHED_TASK_FLAG_OPS(task_flags, idle, __SCHED_TASK_IDLE)
SCHED_TASK_FLAG_OPS(task_flags, resched, __SCHED_TASK_NRESCHED)
SCHED_TASK_FLAG_OPS(task_flags, notify_signal, __SCHED_TASK_SIGNAL)

extern bool sched_cond_resched_handle(void);
extern void sched_preempt_handle(void);

DECLARE_STATIC_CALL(sched_cond_resched, sched_cond_resched_handle);
DECLARE_STATIC_CALL(sched_might_resched, sched_cond_resched_handle);
DECLARE_STATIC_CALL(sched_preempt, sched_preempt_handle);

#define sched_cond_resched()    static_call_cond(sched_cond_resched)()
#define sched_might_resched()   static_call_cond(sched_might_resched)()
#define sched_preempt()         static_call_cond(sched_preempt)()

/* scheduler front end */
extern state sched_task_clone(struct sched_task *task, enum clone_flags flags);
extern struct sched_task *sched_task_create(void);
extern void sched_task_destroy(struct sched_task *task);

static inline void sched_task_get(struct sched_task *task)
{
    refcount_inc(&task->refcount);
}

static inline void sched_task_put(struct sched_task *task)
{
    if (refcount_dec_test(&task->refcount))
        sched_task_destroy(task);
}

/* idle task */
extern void __noreturn idle_task_entry(void);

/* scheduler context */
extern void __noreturn sched_kill(void);
extern void sched_idle(void);
extern void sched_tick(void);
extern void sched_resched(void);
extern void sched_yield(void);

/* scheduler wakeup */
extern void sched_wake_up(struct sched_task *task);
extern void sched_wake_up_new(struct sched_task *task);

/* scheduler block timeout */
extern ttime_t sched_timeout(ttime_t timeout);
extern ttime_t sched_timeout_interruptible(ttime_t timeout);
extern ttime_t sched_timeout_uninterruptible(ttime_t timeout);

/* scheduler framework */
extern struct sched_type *sched_find(const char *name, unsigned char prio);
extern state sched_register(struct sched_type *sched);
extern void sched_unregister(struct sched_type *sched);
extern void __init sched_init(void);

#endif /* _KERNEL_SCHED_H_ */
