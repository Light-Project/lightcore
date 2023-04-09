/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <delay.h>
#include <driver/rng.h>

static state rng_testing(struct kshell_context *ctx, void *pdata)
{
    struct rng_device *rdev;
    state retval = -ENOERR;
    unsigned long rand;

    spin_lock(&rng_lock);
    list_for_each_entry(rdev, &rng_list, list) {
        rng_enable(rdev);
        retval = rng_read(rdev, &rand);
        kshell_printf(ctx, "rng '%s' test read: %lu ret (%d)\n",
                rdev->dev->name, rand, retval);
        if (retval)
            break;
    }
    spin_unlock(&rng_lock);

    return retval;
}

static struct selftest_command rng_command = {
    .group = "driver",
    .name = "rng",
    .desc = "rng unit test",
    .testing = rng_testing,
};

static state selftest_rng_init(void)
{
    return selftest_register(&rng_command);
}
kshell_initcall(selftest_rng_init);
