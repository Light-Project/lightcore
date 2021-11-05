/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_SCHED_H_
#define _ASM_GENERIC_SCHED_H_

#include <sched.h>
#include <asm/percpu.h>

DECLARE_PERCPU(struct sched_queue, queues);

static inline struct task *sched_current(void)
{
    return thiscpu_ptr(&queues)->curr;
}
#define current sched_current()

#endif /* _ASM_GENERIC_PERCPU_H_ */
