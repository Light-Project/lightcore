/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "pevent: " fmt

#include <pevent.h>
#include <spinlock.h>
#include <export.h>
#include <printk.h>

static LIST_HEAD(pevent_list);
static SPIN_LOCK(pevent_lock);

/**
 * pevent_suspend - execute all registered pevent suspend callbacks.
 */
state pevent_suspend(void)
{
    struct pevent *pevent;
    state ret;

    list_for_each_entry(pevent, &pevent_list, list) {
        if (!pevent->suspend)
            continue;
        pr_debug("suspend calling %p\n", pevent->suspend);
        ret = pevent->suspend();
        if (ret)
            goto error;
    }

    return -ENOERR;

error:
    pr_err("suspend calling %p fail", pevent->suspend);

    list_for_each_entry_reverse_continue(pevent, &pevent_list, list)
        if (!pevent->resume)
            pevent->resume();

    return ret;
}

/**
 * pevent_resume - execute all registered pevent resume callbacks.
 */
void pevent_resume(void)
{
    struct pevent *pevent;

    list_for_each_entry(pevent, &pevent_list, list) {
        if (!pevent->shutdown)
            continue;
        pr_debug("resume calling %p\n", pevent->resume);
        pevent->resume();
    }
}

/**
 * pevent_shutdown - execute all registered pevent shutdown callbacks.
 */
void pevent_shutdown(void)
{
    struct pevent *pevent;

    spin_lock(&pevent_lock);

    list_for_each_entry(pevent, &pevent_list, list) {
        if (!pevent->shutdown)
            continue;
        pr_debug("shutdown calling %p\n", pevent->shutdown);
        pevent->shutdown();
    }

    spin_unlock(&pevent_lock);
}

/**
 * pevent_register - register a set of pevent operations.
 * @pevent: pevent operations to register.
 */
state pevent_register(struct pevent *pevent)
{
    if (!pevent->shutdown && (!pevent->suspend && !pevent->resume))
        return -EINVAL;

    list_add_prev(&pevent_list, &pevent->list);
    return -ENOERR;
}
EXPORT_SYMBOL(pevent_register);

/**
 * pevent_unregister - unregister a set of pevent operations.
 * @pevent: pevent operations to unregister.
 */
void pevent_unregister(struct pevent *pevent)
{
    list_del(&pevent->list);
}
EXPORT_SYMBOL(pevent_unregister);
