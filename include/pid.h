/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _PID_H_
#define _PID_H_

#include <types.h>
#include <state.h>
#include <namespace.h>

struct pid {
    pid_t pid;
};

extern state pid_alloc_node(struct namespace_pid *ns, struct pid *pid);
extern struct pid *pid_alloc(struct namespace_pid *ns);
extern void __nonnull(1) pid_release(struct pid *pid);

#endif  /* _PID_H_ */
