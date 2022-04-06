/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
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

static void pvpanic_panic(const void *pdata)
{
    pvpanic_event_send(PVPANIC_PANICKED);
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

static struct panic_work pvpanic_work = {
    .work = pvpanic_panic,
};

static state pvpanic_init(void)
{
    return panic_work_register(&pvpanic_work);
}
driver_initcall(pvpanic_init);
