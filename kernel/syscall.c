/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <syscall.h>
#include <linkage.h>
#include <dynamic-call.h>
#include <spinlock.h>
#include <atomic.h>
#include <printk.h>
#include <export.h>

static RB_ROOT(syscall_root);
static RW_LOCK(syscall_lock);

static long syscall_rb_find(const struct rb_node *rb, const void *pdata)
{
    struct syscall_desc *desc = node_to_rbtree(rb);
    if (desc->callnr == (unsigned long)pdata) return 0;
    return desc->callnr < (unsigned long)pdata ? -1 : 1;
}

static long syscall_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    struct syscall_desc *desca = node_to_rbtree(rba);
    struct syscall_desc *descb = node_to_rbtree(rbb);
    if (desca->callnr == descb->callnr) return 0;
    return desca->callnr < descb->callnr ? -1 : 1;
}

asmlinkage long syscall_handle(unsigned long callnr, long arg1, long arg2,
                               long arg3, long arg4, long arg5, long arg6)
{
    struct syscall_desc *syscall;
    struct rb_node *node;
    long retval;

    read_lock(&syscall_lock);
    node = rb_find(&syscall_root, (void *)callnr, syscall_rb_find);
    read_unlock(&syscall_lock);

    if (unlikely(!node)) {
        pr_info("unexpected syscall%ld\n", callnr);
        return -EINVAL;
    }

    syscall = node_to_rbtree(node);
    retval = dynamic_call(syscall->entry, callnr,
        (void **)(long []) {
            arg1, arg2, arg3,
            arg4, arg5, arg6,
        }
    );

    atomic_inc(&syscall->count);
    return retval;
}

state syscall_register(struct syscall_desc *syscall)
{
    bool conflict;

    if (!syscall->entry || syscall->argnr > 6)
        return -EINVAL;

    write_lock(&syscall_lock);
    conflict = rb_insert_conflict(&syscall_root, &syscall->node, syscall_rb_cmp);
    write_unlock(&syscall_lock);

    return conflict ? -EBUSY : -ENOERR;
}
EXPORT_SYMBOL(syscall_register);

void syscall_unregister(struct syscall_desc *syscall)
{
    write_lock(&syscall_lock);
    rb_delete(&syscall_root, &syscall->node);
    write_unlock(&syscall_lock);
}
EXPORT_SYMBOL(syscall_unregister);
