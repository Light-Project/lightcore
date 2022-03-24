/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <namespace.h>

struct namespace root_ns = {
    .pid = &root_pid,
};

static struct kcache *ns_cache;

struct namespace *namespace_create(struct sched_task *task)
{
    struct namespace *ns;

    ns = kcache_zalloc(ns_cache, GFP_KERNEL);
    if (!ns)
        return NULL;

    return ns;
}

void __init namespace_init(void)
{
    ns_cache = kcache_create(
        "namespace",
        sizeof(root_ns), KCACHE_PANIC
    );
}
