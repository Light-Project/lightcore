/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <spinlock.h>
#include <timer.h>
#include <completion.h>
#include <selftest.h>

#define TEST_LOOP   10
#define TEST_DELAY  100

static void timer_handle(void *pdata)
{
    struct completion *comp = pdata;
    complete_one(comp);
}

static state timer_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int count;

    DEFINE_COMPLETION(timer_waiter);
    DEFINE_TIMER(timer, timer_handle, &timer_waiter, TEST_DELAY, 0);

    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "timer test%02u: ", count);
        timer_pending(&timer);
        completion_wait(&timer_waiter);
        kshell_puts(ctx, "handled\n");
    }

    return -ENOERR;
}

static struct selftest_command timer_command = {
    .group = "kernel",
    .name = "timer",
    .desc = "timer unit test",
    .testing = timer_testing,
};

static state selftest_timer_init(void)
{
    return selftest_register(&timer_command);
}
kshell_initcall(selftest_timer_init);
