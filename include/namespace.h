/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_

#include <types.h>
#include <kmalloc.h>
#include <refcount.h>
#include <idr.h>

struct namespace_pid {
    struct namespace_pid *parent;
    refcount_t refcount;
    unsigned int level;
    struct kcache *cache;
    struct idr_root pids;
};

struct namespace_ipc {

};

struct namespace {
    refcount_t refcount;
    struct namespace_pid *pid;
    struct namespace_ipc *ipc;
};

extern struct namespace_pid root_pid;
extern struct namespace_ipc root_ipc;
extern struct namespace root_ns;

extern void __init namespace_init(void);

#endif  /* _NAMESPACE_H_ */
