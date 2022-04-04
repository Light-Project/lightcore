/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _PID_H_
#define _PID_H_

#include <namespace.h>
#include <hlist.h>

struct sched_task;

/**
 * enum pid_type - id type
 * @pid: process id
 * @pgid: process group id
 * @tgid: thread group id
 * @sid: session id
 */
enum pid_type {
    PID_TYPE_PID,
    PID_TYPE_PGID,
    PID_TYPE_TGID,
    PID_TYPE_SID,
    PID_TYPE_MAX,
};

/**
 * struct upid - represents a pid number
 * @ns: the namespace of the current PID number.
 * @pid: current PID number.
 */
struct upid {
    struct namespace_pid *ns;
    pid_t pid;
};

/**
 * struct pid - represents a pid node
 * @lock: lock of PID node.
 * @count: reference count of PID node.
 * @level: total number of namespace layers of PID node.
 * @tasks: linked list of different types of tasks of PID node.
 * @pids: multi tier namespace of PID node.
 */
struct pid {
    spinlock_t lock;
    refcount_t count;
    unsigned int level;
    struct hlist_head tasks[PID_TYPE_MAX];
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

static inline bool pid_check_empty(struct pid *pid, enum pid_type type)
{
    return hlist_check_empty(&pid->tasks[type]);
}

extern void pid_get(struct pid *pid);
extern void pid_put(struct pid *pid);
extern struct pid *pid_find(pid_t nr);
extern struct pid *pid_find_get(pid_t nr);

extern void pid_attach(struct sched_task *task, enum pid_type type);
extern void pid_detach(struct sched_task *task, enum pid_type type);

extern struct pid *pid_alloc(struct namespace_pid *ns, pid_t *tid, size_t tid_size);
extern void pid_free(struct pid *pid);
extern void __init pid_init(void);

#endif  /* _PID_H_ */
