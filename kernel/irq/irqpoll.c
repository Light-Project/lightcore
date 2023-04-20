/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <irqpoll.h>
#include <cpu.h>
#include <percpu.h>
#include <ticktime.h>
#include <softirq.h>
#include <crash.h>
#include <export.h>

static unsigned int __read_mostly irqpoll_capacity = IRQPOLL_CAPACITY;
static DEFINE_PERCPU(struct list_head, irqpoll_lists);

static void irqpoll_softirq_handle(void *pdata);
static DEFINE_SOFTIRQ(irqpoll_softirq, irqpoll_softirq_handle, NULL, 0);

static void irqpoll_complete_irqoff(struct irqpoll *poll)
{
    list_del(&poll->list);
    irqpoll_clr_queue(poll);
    complete_all(&poll->comp);
}

void irqpoll_complete(struct irqpoll *poll)
{
    irqflags_t irqflags;

    irqflags = irq_local_save();
    irqpoll_complete_irqoff(poll);
    irq_local_restore(irqflags);
}
EXPORT_SYMBOL(irqpoll_complete);

static void irqpoll_softirq_handle(void *pdata)
{
    struct list_head *irqpoll_list = thiscpu_ptr(&irqpoll_lists);
    unsigned int budget = irqpoll_capacity;
    ttime_t start_time = ticktime;

    irq_local_disable();

    while (!list_check_empty(irqpoll_list)) {
        unsigned int weight, work;
        struct irqpoll *poll;

        if (unlikely(!budget || ttime_after(ticktime, start_time))) {
            softirq_pending(&irqpoll_softirq);
            break;
        }

        irq_local_enable();

        poll = list_first_entry(irqpoll_list, struct irqpoll, list);
        weight = poll->capacity;
        work = 0;

        if (irqpoll_test_queue(poll))
            work = poll->entry(weight, poll->pdata);

        if (work >= budget)
            work = budget;

        budget -= work;

        irq_local_disable();

        if (work >= weight) {
            if (irqpoll_test_kill(poll))
                irqpoll_complete_irqoff(poll);
            else
                list_move_prev(irqpoll_list, &poll->list);
        }
    }

    irq_local_enable();
}

void irqpoll_enable(struct irqpoll *poll)
{
    WARN_ON(!irqpoll_test_kill(poll));
    irqpoll_clr_kill(poll);
}
EXPORT_SYMBOL(irqpoll_enable);

void irqpoll_disable(struct irqpoll *poll)
{
    irqpoll_set_kill(poll);
    while (irqpoll_test_queue(poll))
        completion_wait(&poll->comp);
}
EXPORT_SYMBOL(irqpoll_disable);

state irqpoll_enqueue(struct irqpoll *poll)
{
    struct list_head *irqpoll_list = thiscpu_ptr(&irqpoll_lists);
    irqflags_t irqflags;

    if (!poll->entry || !poll->capacity)
        return -EINVAL;

    if (irqpoll_test_kill(poll))
        return -EBUSY;

    if (irqpoll_test_set_queue(poll))
        return -EALREADY;

    completion_init(&poll->comp);

    irqflags = irq_local_save();
    list_add_prev(irqpoll_list, &poll->list);
    softirq_pending(&irqpoll_softirq);
    irq_local_restore(irqflags);

    return -ENOERR;
}
EXPORT_SYMBOL(irqpoll_enqueue);

void __init irqpoll_setup(void)
{
    struct list_head *list;
    unsigned int cpu;

    cpu_for_each(cpu) {
        list = percpu_ptr(cpu, &irqpoll_lists);
        list_head_init(list);
    }

    softirq_register(&irqpoll_softirq);
}
