/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "refcount-debug"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <refcount.h>
#include <printk.h>
#include <panic.h>

void refcount_saturation_report(refcount_t *ref, enum refcount_saturation type)
{
    switch (type) {
        case REFCOUNT_ADDNZ_OVF:
            pr_crit("refcount (%p) addnz overflow\n", ref);
            break;

        case REFCOUNT_ADDNZ_LEAK:
            pr_crit("refcount (%p) addnz leaking\n", ref);
            break;

        case REFCOUNT_ADD_UAF:
            pr_crit("refcount (%p) add use-after-free\n", ref);
            break;

        case REFCOUNT_ADD_OVF:
            pr_crit("refcount (%p) add overflow\n", ref);
            break;

        case REFCOUNT_ADD_LEAK:
            pr_crit("refcount (%p) add leaking\n", ref);
            break;

        case REFCOUNT_SUB_UAF:
            pr_crit("refcount (%p) sub use-after-free\n", ref);
            break;

        case REFCOUNT_SUB_OVF:
            pr_crit("refcount (%p) sub overflow\n", ref);
            break;

        case REFCOUNT_SUB_LEAK:
            pr_crit("refcount (%p) sub leaking\n", ref);
            break;

        default:
            return;
    }

    BUG();
}
