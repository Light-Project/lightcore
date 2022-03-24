/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _PID_H_
#define _PID_H_

#include <namespace.h>

struct upid {
    struct namespace_pid *ns;
    pid_t pid;
};

/**
 * struct pid -
 *
 *
 */
struct pid {
    spinlock_t lock;
    refcount_t count;
    unsigned int level;
    struct upid pids[1];
};

extern struct pid init_task_pid;

static inline pid_t pid_ns_nr(struct namespace_pid *ns, struct pid *pid)
{
    struct upid *upid;

    if (pid && ns->level <= pid->level) {
        upid = &pid->pids[ns->level];
        if (upid->ns == ns)
            return upid->pid;
    }

    return 0;
}

extern void pid_get(struct pid *pid);
extern void pid_put(struct pid *pid);
extern struct pid *pid_alloc(struct namespace_pid *ns, pid_t *tid, size_t tid_size);
extern void pid_release(struct pid *pid);
extern void __init pid_init(void);

#endif  /* _PID_H_ */
