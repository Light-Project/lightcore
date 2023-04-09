/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pvpanic"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include "pvpanic.h"
#include <initcall.h>
#include <spinlock.h>
#include <ioops.h>
#include <panic.h>
#include <export.h>

static LIST_HEAD(pvpanic_list);
static SPIN_LOCK(pvpanic_lock);

static void pvpanic_event_send(uint8_t event)
{
    struct pvpanic_device *pdev;

    spin_lock(&pvpanic_lock);
    list_for_each_entry(pdev, &pvpanic_list, list) {
        if (event & pdev->capability & pdev->events)
            mpio_writeb(pdev->base, event);
    }
    spin_unlock(&pvpanic_lock);
}

static notifier_return_t pvpanic_panic(struct notifier_node *node, void *arg)
{
    pvpanic_event_send(PVPANIC_PANICKED);
    return NOTIFI_RET_DONE;
}

state pvpanic_probe(struct pvpanic_device *pdev)
{
    spin_lock(&pvpanic_lock);
    list_add(&pvpanic_list, &pdev->list);
    spin_unlock(&pvpanic_lock);
    return -ENOERR;
}
EXPORT_SYMBOL(pvpanic_probe);

void pvpanic_remove(struct pvpanic_device *pdev)
{
    spin_lock(&pvpanic_lock);
    list_del(&pdev->list);
    spin_unlock(&pvpanic_lock);
}
EXPORT_SYMBOL(pvpanic_remove);

static struct notifier_node pvpanic_work = {
    .entry = pvpanic_panic,
    .priority = INT_MAX,
};

static state pvpanic_init(void)
{
    return notifier_spin_chain_register(&panic_notifier, &pvpanic_work);
}
driver_initcall(pvpanic_init);
