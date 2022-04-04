/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <pid.h>
#include <idr.h>
#include <export.h>

struct pid init_task_pid = {
    .level = 0,
};

struct namespace_pid root_pid = {
    .level = 0,
};

#define PID_RESERVED 400

static SPIN_LOCK(pid_lock);
static pid_t pid_min = PID_RESERVED + 1;
static pid_t pid_max = CONFIG_PID_MAX_DEFAULT;

/**
 * pid_get - Get a PID node
 * @pid:
 */
void pid_get(struct pid *pid)
{
    if (!pid)
        return;
    refcount_inc(&pid->count);
}
EXPORT_SYMBOL(pid_get);

/**
 * pid_put - Put a PID node
 * @pid:
 */
void pid_put(struct pid *pid)
{
    struct namespace_pid *ns;

    if (!pid)
        return;

    ns = pid->pids[pid->level].ns;
    if (refcount_dec_test(&pid->count))
        kcache_free(ns->cache, pid);
}
EXPORT_SYMBOL(pid_put);

/**
 * pid_alloc - Allocation a PID node
 * @ns: the namespace to allocation in.
 * @tid: array to specify PID.
 * @tid_size: array size of @tid.
 */
struct pid *pid_alloc(struct namespace_pid *ns, pid_t *tid, size_t tid_size)
{
    struct namespace_pid *walk;
    unsigned int level;
    struct pid *pid;
    struct upid *upid;
    state ret = -ENOERR;

    pid = kcache_zalloc(ns->cache, GFP_KERNEL);
    if (!pid)
        return NULL;

    for (walk = ns, level = ns->level; level + 1; --level) {
        pid_t num, tnr = 0;

        if (tid_size--) {
            tnr = tid[ns->level - level];

            if (tnr < 1 || tnr >= pid_max) {
                ret = -EINVAL;
                goto error_free;
            }
        }

        spin_lock(&pid_lock);
        if (tnr) {
            num = idr_alloc_cyclic_range(
                &ns->pids, NULL, tnr + 0, tnr + 1, GFP_KERNEL
            );
        } else {
            num = idr_alloc_cyclic_range(
                &ns->pids, NULL, pid_min, pid_max, GFP_KERNEL
            );
        }
        spin_unlock(&pid_lock);

        if (num == IDR_NONE) {
            ret = -ENOSPC;
            goto error_free;
        }

        pid->pids[level].pid = num;
        pid->pids[level].ns = walk;
        walk = walk->parent;
    }

    pid->level = ns->level;
    spin_lock_init(&pid->lock);
    refcount_set(&pid->count, 1);

    return pid;

error_free:
    spin_lock(&pid_lock);

    while (++level <= ns->level) {
        upid = pid->pids + level;
        idr_free(&ns->pids, upid->pid);
    }

    spin_unlock(&pid_lock);
    kcache_free(ns->cache, pid);

    return ERR_PTR(ret);
}
EXPORT_SYMBOL(pid_alloc);

/**
 * pid_free -  Release a PID node
 * @pid:
 */
void pid_free(struct pid *pid)
{
    struct namespace_pid *ns;
    struct upid *upid;
    unsigned int count;

    spin_lock(&pid_lock);
    for (count = 0; count <= pid->level; ++count) {
        upid = &pid->pids[count];
        ns = upid->ns;
        idr_free(&ns->pids, upid->pid);
    }
    spin_unlock(&pid_lock);
}
EXPORT_SYMBOL(pid_free);

state pid_namespace_clone(struct namespace_pid *new, struct namespace_pid *old)
{
    return -ENOERR;
}

void __init pid_init(void)
{
    root_pid.cache = KCACHE_CREATE(struct pid, KCACHE_PANIC);
    idr_root_init(&root_pid.pids, 0);
}
