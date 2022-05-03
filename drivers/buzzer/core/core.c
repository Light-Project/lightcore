/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "buzzer"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <driver/buzzer.h>
#include <printk.h>
#include <panic.h>
#include <export.h>

LIST_HEAD(buzzer_list);
SPIN_LOCK(buzzer_lock);
static struct notifier_node buzzer_work;

struct buzzer_device *default_buzzer;
EXPORT_SYMBOL(default_buzzer);

static notifier_return_t buzzer_panic(struct notifier_node *node, void *arg)
{
    struct buzzer_device *bdev;
    struct buzzer_ops *ops;

    if ((bdev = default_buzzer)) {
        ops = bdev->ops;
        if (ops->panic) {
            ops->panic(bdev);
            return NOTIFI_RET_DONE;
        }
    }

    list_for_each_entry(bdev, &buzzer_list, list) {
        ops = bdev->ops;
        if (ops->panic) {
            ops->panic(bdev);
            return NOTIFI_RET_DONE;
        }
    }

    return NOTIFI_RET_WARN;
}

state buzzer_register(struct buzzer_device *bdev)
{
    if (!bdev->dev || !bdev->ops)
        return -ENOERR;

    spin_lock(&buzzer_lock);
    list_add(&buzzer_list, &bdev->list);
    spin_unlock(&buzzer_lock);

    WRITE_ONCE(default_buzzer, bdev);
    return -ENOERR;
}

void buzzer_unregister(struct buzzer_device *bdev)
{
    buzzer_stop(bdev);

    spin_lock(&buzzer_lock);
    list_del(&bdev->list);
    spin_unlock(&buzzer_lock);
}

static state buzzer_init(void)
{
    buzzer_work.entry = buzzer_panic;
    return notifier_spin_chain_register(&panic_notifier, &buzzer_work);
}
framework_initcall(buzzer_init);
