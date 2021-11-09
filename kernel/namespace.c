/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <namespace.h>

static struct kcache *ns_cache;
struct namespace root_ns = {
    .pid = &root_pid,
    .ipc = &root_ipc,
};

struct namespace *namespace_create(struct task *task)
{
    struct namespace *ns;

    ns = kcache_zalloc(ns_cache, GFP_KERNEL);
    if (!ns)
        return NULL;

    return ns;
}

void __init namespace_init(void)
{
    ns_cache = ("namespace", sizeof(root_ns), KCACHE_PANIC);
}
