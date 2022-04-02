/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kcoro.h>
#include <delay.h>
#include <selftest.h>

#define TEST_LOOP 100

static state kcoro_task_a(void *pdata)
{
    unsigned int count = TEST_LOOP;

    while (count--) {
        kshell_printf("kcoro task a running...\n");
        kcoro_yield();
    }

    kcoro_exit();
    return -ENOERR;
}

static state kcoro_task_b(void *pdata)
{
    unsigned int count = TEST_LOOP;

    while (count--) {
        kshell_printf("kcoro task b running...\n");
        mdelay(10);
        kcoro_yield();
    }

    kcoro_exit();
    return -ENOERR;
}

static state kcoro_task_c(void *pdata)
{
    unsigned int count = TEST_LOOP;

    while (count--) {
        kshell_printf("kcoro task c running...\n");
        kcoro_msleep(10);
        kcoro_yield();
    }

    kcoro_exit();
    return -ENOERR;
}

static state kcoro_testing(void *pdata)
{
    struct kcoro_worker *worker;
    struct kcoro_work *work_a;
    struct kcoro_work *work_b;
    struct kcoro_work *work_c;
    state ret;

    worker = kcoro_worker_create("kcoro test");
    if (unlikely(ret = PTR_ERR(worker)))
        return ret;

    work_a = kcoro_work_create(worker, kcoro_task_a, NULL, "kcoro test task a");
    work_b = kcoro_work_create(worker, kcoro_task_b, NULL, "kcoro test task b");
    work_c = kcoro_work_create(worker, kcoro_task_c, NULL, "kcoro test task b");

    if ((ret = PTR_ERR(work_a)) ||
        (ret = PTR_ERR(work_b)) ||
        (ret = PTR_ERR(work_c))
    ) {
        kcoro_worker_destroy(worker);
        return ret;
    }

    kcoro_dispatch(worker);
    kcoro_worker_destroy(worker);

    return -ENOERR;
}

static struct selftest_command kcoro_command = {
    .group = "kernel",
    .name = "kcoro",
    .desc = "kcoro unit test",
    .testing = kcoro_testing,
};

static state selftest_kcoro_init(void)
{
    return selftest_register(&kcoro_command);
}
kshell_initcall(selftest_kcoro_init);
