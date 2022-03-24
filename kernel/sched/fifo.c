/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "fifo-sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <sched.h>
#include <bitmap.h>
#include <kmalloc.h>
#include <initcall.h>
#include <printk.h>

#define FIFO_PRIO_MAX 64
static struct kcache *fifo_cache;

struct fifo_queue {
    DEFINE_BITMAP(bitmap, FIFO_PRIO_MAX);
    struct list_head task[FIFO_PRIO_MAX];
    unsigned int running[FIFO_PRIO_MAX];
    struct sched_queue *sched;
};

struct fifo_task {
    struct sched_task task;
    struct list_head list;
};

#define task_to_fifo(tsk) \
    container_of(tsk, struct fifo_task, task)

static __always_inline struct fifo_queue *
queue_get_fifo(struct sched_queue *queue)
{
    return queue->pdata[SCHED_PRIO_FIFO];
}

static __always_inline void
queue_set_fifo(struct sched_queue *queue, struct fifo_queue *fifo)
{
    queue->pdata[SCHED_PRIO_FIFO] = fifo;
}

static __always_inline bool
move_entity(enum sched_queue_flags flags)
{
    return !((flags & (SCHED_QUEUE_SAVE | SCHED_QUEUE_MOVE)) == SCHED_QUEUE_SAVE);
}

static state fifo_queue_create(struct sched_queue *queue)
{
    struct fifo_queue *fifo_queue;

    fifo_queue = kzalloc(sizeof(*fifo_queue), GFP_KERNEL);
    if (!fifo_queue)
        return -ENOMEM;

    list_head_init(fifo_queue->task);
    queue_set_fifo(queue, fifo_queue);

    return -ENOERR;
}

static void fifo_queue_destroy(struct sched_queue *queue)
{
    struct fifo_queue *fifo_queue = queue_get_fifo(queue);
    kfree(fifo_queue);
}

static void fifo_task_enqueue(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    struct fifo_queue *fifo_queue = queue_get_fifo(queue);
    struct fifo_task *fifo_task = task_to_fifo(task);
    unsigned int prio = task->priority;

    if (move_entity(flags)) {
        if (flags & SCHED_ENQUEUE_HEAD)
            list_add(&fifo_queue->task[prio], &fifo_task->list);
        else
            list_add_prev(&fifo_queue->task[prio], &fifo_task->list);
        bit_set(fifo_queue->bitmap, task->priority);
    }

    fifo_queue->running[prio]++;
}

static void fifo_task_dequeue(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    struct fifo_queue *fifo_queue = queue_get_fifo(queue);
    struct fifo_task *fifo_task = task_to_fifo(task);
    unsigned int prio = task->priority;

    if (move_entity(flags)) {
        list_del(&fifo_task->list);
        if (list_check_empty(&fifo_queue->task[prio]))
            bit_clr(fifo_queue->bitmap, task->priority);
    }

    fifo_queue->running[prio]--;
}

static struct sched_task *fifo_task_create(int numa)
{
    struct fifo_task *fifo_task;

    fifo_task = kcache_alloc_numa(fifo_cache, GFP_KERNEL, numa);
    if (!fifo_task)
        return NULL;

    fifo_task->task.priority = FIFO_PRIO_MAX >> 1;
    return &fifo_task->task;
}

static void fifo_task_destroy(struct sched_task *task)
{
    struct fifo_task *fifo_task = task_to_fifo(task);
    list_del(&fifo_task->list);
    kcache_free(fifo_cache, fifo_task);
}

static void fifo_task_next(struct sched_queue *queue, struct sched_task *task)
{

}

static void fifo_task_prev(struct sched_queue *queue, struct sched_task *task)
{

}

static void fifo_task_yield(struct sched_queue *queue)
{
    struct fifo_queue *fifo_queue = queue_get_fifo(queue);
    struct fifo_task *fifo_task = task_to_fifo(queue->curr);
    unsigned int prio = fifo_task->task.priority;
    list_move_tail(&fifo_queue->task[prio], &fifo_task->list);
}

static struct sched_task *fifo_task_pick(struct sched_queue *queue)
{
    struct fifo_queue *fifo_queue = queue_get_fifo(queue);
    struct fifo_task *fifo_task;
    struct list_head *prio;
    unsigned int index;

    index = find_first_bit(fifo_queue->bitmap, FIFO_PRIO_MAX);
    if ((index >= FIFO_PRIO_MAX))
        return NULL;

    prio = fifo_queue->task + index;
    fifo_task = list_first_entry(prio, struct fifo_task, list);

    return &fifo_task->task;
}

static struct sched_type fifo_sched = {
    .name = MODULE_NAME,
    .priority = SCHED_PRIO_FIFO,

    .queue_create = fifo_queue_create,
    .queue_destroy = fifo_queue_destroy,
    .task_enqueue = fifo_task_enqueue,
    .task_dequeue = fifo_task_dequeue,

    .task_create = fifo_task_create,
    .task_destroy = fifo_task_destroy,
    .task_next = fifo_task_next,
    .task_prev = fifo_task_prev,
    .task_yield = fifo_task_yield,
    .task_pick = fifo_task_pick,
};

static state fifo_sched_init(void)
{
#ifdef CONFIG_SCHED_DEF_FIFO
    default_sched = &fifo_sched;
#endif
    fifo_cache = KCACHE_CREATE(struct fifo_task, KCACHE_PANIC);
    return sched_register(&fifo_sched);
}
scheduler_initcall(fifo_sched_init);