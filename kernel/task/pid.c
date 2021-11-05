/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <pid.h>

struct namespace_pid root_pid;

state pid_alloc_node(struct namespace_pid *ns, struct pid *pid)
{


    return -ENOERR;
}

struct pid *pid_alloc(struct namespace_pid *ns)
{
    struct pid *pid;

    pid = kcache_alloc(ns->cache, GFP_KERNEL);

    return pid;
}

void pid_release(struct pid *pid)
{

}

state pid_namespace_create(struct namespace_pid *new, struct namespace_pid *old)
{
    return -ENOERR;
}
