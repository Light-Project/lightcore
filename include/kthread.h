/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTHREAD_H_
#define _KTHREAD_H_

#include <state.h>
#include <sched.h>

typedef state (*kthread_t)(void *data);

__printf(2, 3) struct task *
kthread_create(kthread_t kthread, void *data, const char *name, ...);

#endif  /* _KTHREAD_H_ */
