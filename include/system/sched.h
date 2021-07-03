/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SYSTEM_SCHED_H_
#define _SYSTEM_SCHED_H_

#include <list.h>

struct task {

    /* Task structure description */
    struct list_head buddy;     /* buddy task */
    struct list_head children;  /* children of this task */

    intmax_t pid;   /* Process ID */

    struct task_mm  *mm;

    void *stack;

#ifdef CONFIG_SMP
    int cpu_nr;     /* On which CPU */
#endif /* CONFIG_SMP */

    uint64_t    kernel_task:1;  /*  */

};

void sched_relax(void);

#endif /* _SYSTEM_SCHED_H_ */
