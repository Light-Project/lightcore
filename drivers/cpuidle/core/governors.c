/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "cpuidle-gov"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ticktime.h>
#include <driver/cpuidle.h>
#include <printk.h>
#include <crash.h>
#include <export.h>

LIST_HEAD(cpuidle_governors);
struct cpuidle_governor *cpuidle_governor_curr;

/**
 * cpuidle_governor_find - finds a governor by specified name.
 * @name: the governor name to find out.
 * NOTE: Must be called with cpuidle_lock acquired.
 */
struct cpuidle_governor *cpuidle_governor_find(const char *name)
{
    struct cpuidle_governor *find;

    list_for_each_entry(find, &cpuidle_governors, list) {
        if (!strcmp(find->name, name))
            return find;
    }

    return NULL;
}

/**
 * cpuidle_governor_switch - changes the governor.
 * @governor: the governor to switch to.
 * NOTE: Must be called with cpuidle_lock acquired.
 */
void cpuidle_governor_switch(struct cpuidle_governor *governor)
{
    if (governor == cpuidle_governor_curr)
        return;

    cpuidle_governor_curr = governor;
    pr_info("using governor '%s'\n", governor->name);
}

/**
 * cpuidle_governor_register - register a cpuidle governor.
 * @governor: the governor to register.
 */
state cpuidle_governor_register(struct cpuidle_governor *governor)
{
    struct list_head *head = &cpuidle_governors;
    struct cpuidle_governor *walk;

    if (!governor->select)
        return -EINVAL;

    mutex_lock(&cpuidle_lock);
    if (cpuidle_governor_find(governor->name)) {
        mutex_unlock(&cpuidle_lock);
        pr_warn("governor '%s' already registered\n", governor->name);
        return -EALREADY;
    }

    /* insert by rating. */
    list_for_each_entry(walk, &cpuidle_governors, list) {
        if (walk->rating < governor->rating)
            break;
        head = &walk->list;
    }

    list_add(head, &governor->list);
    pr_debug("governor '%s' registered\n", governor->name);

    if (head == &cpuidle_governors)
        cpuidle_governor_switch(governor);
    mutex_unlock(&cpuidle_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(cpuidle_governor_register);

/**
 * cpuidle_governor_unregister - unregister a cpuidle governor.
 * @governor: the governor to unregister.
 */
void cpuidle_governor_unregister(struct cpuidle_governor *governor)
{
    struct cpuidle_governor *best;

    mutex_lock(&cpuidle_lock);
    list_del(&governor->list);
    BUG_ON(!list_check_another(&cpuidle_governors, &governor->list));

    if (governor == cpuidle_governor_curr) {
        best = list_first_entry(&cpuidle_governors, struct cpuidle_governor, list);
        cpuidle_governor_switch(best);
    }

    pr_debug("governor '%s' unregistered\n", governor->name);
    mutex_unlock(&cpuidle_lock);
}
EXPORT_SYMBOL(cpuidle_governor_unregister);
