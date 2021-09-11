/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <sched.h>
#include <asm/proc.h>

static LIST_HEAD(sched_list);

struct sched_type *sched_find(const char *name)
{
    struct sched_type *sched;

    list_for_each_entry(sched, &sched_list, list)
        if (!strcmp(sched->name, name))
            return sched;

    return NULL;
}

state sched_register(struct sched_type *sched)
{
    list_add(&sched_list, &sched->list);
    return -ENOERR;
}

void sched_unregister(struct sched_type *sched)
{
    list_del(&sched->list);
}

void sched_set(struct task *task, const char *name)
{
    struct sched_type *sched = sched_find(name);
    sched->task_setup(task);
}

void sched_relax(void)
{

}

/**
 * context_switch - switch context form prev to next
 * @prev: switch form
 * @next: switch to
 */
void context_switch(struct task *prev, struct task *next)
{

    if(!next->mm) {     /* switch to kernel */

    } else {            /* switch to user */

    }

}


void __init sched_init(void)
{

}
