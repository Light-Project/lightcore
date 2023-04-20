/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <irqpoll.h>

#define TEST_LOOP 100

struct irqpoll_test {
    struct irqpoll poll;
    struct kshell_context *ctx;
};

static unsigned int irqpoll_handle(unsigned int weight, void *pdata)
{
    struct irqpoll_test *test = pdata;
    kshell_printf(test->ctx, "irqpoll handle: %u\n", weight);
    irqpoll_complete(&test->poll);
    return 0;
}

static state irqpoll_testing(struct kshell_context *ctx, void *pdata)
{
    struct irqpoll_test test;
    unsigned int count;

    test.ctx = ctx;
    irqpoll_init(&test.poll, IRQPOLL_CAPACITY, irqpoll_handle, &test);

    for (count = 0; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "irqpoll test: %u\n", count);
        irqpoll_enqueue(&test.poll);
        irqpoll_disable(&test.poll);
        irqpoll_enable(&test.poll);
    }

    return -ENOERR;
}

static struct selftest_command irqpoll_command = {
    .group = "kernel",
    .name = "irqpoll",
    .desc = "irqpoll unit test",
    .testing = irqpoll_testing,
};

static state selftest_irqpoll_init(void)
{
    return selftest_register(&irqpoll_command);
}
kshell_initcall(selftest_irqpoll_init);
