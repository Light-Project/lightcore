/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <timekeeping.h>
#include <kcoro.h>
#include <delay.h>
#include <selftest.h>

#define TEST_LOOP 100

struct kcoro_pdata {
    struct kshell_context *ctx;
    struct kcoro_work *testa;
    struct kcoro_work *testb;
    struct kcoro_work *testc;
    struct kcoro_work *testd;
};

static state work_testa(struct kcoro_work *work, void *pdata)
{
    struct kcoro_pdata *kdata = pdata;
    unsigned int count = TEST_LOOP;

    /* Sequential activate */
    kcoro_activate(work->worker, kdata->testb);

    /* Simulate io intensive scenarios */
    while (count--) {
        kshell_printf(kdata->ctx, "kcoro task-a running...\n");
        kshell_printf(kdata->ctx, "ktime: %lld\n", ktime_to_ns(timekeeping_get_time()));
        kshell_printf(kdata->ctx, "vtime: %lld\n", work->vtime);
        kcoro_yield(work);
    }

    /* Call exit function */
    kcoro_exit(work, -ENOERR);
}

static state work_testb(struct kcoro_work *work, void *pdata)
{
    struct kcoro_pdata *kdata = pdata;
    unsigned int count = TEST_LOOP;

    /* Sequential activate */
    kcoro_activate(work->worker, kdata->testc);

    /* Simulate cpu intensive scenarios */
    while (count--) {
        kshell_printf(kdata->ctx, "kcoro task-b running...\n");
        kshell_printf(kdata->ctx, "ktime: %lld\n", ktime_to_ns(timekeeping_get_time()));
        kshell_printf(kdata->ctx, "vtime: %lld\n", work->vtime);
        mdelay(20);
        kcoro_yield(work);
    }

    /* Call exit function */
    kcoro_exit(work, -ENOERR);
}

static state work_testc(struct kcoro_work *work, void *pdata)
{
    struct kcoro_pdata *kdata = pdata;
    unsigned int count = TEST_LOOP;

    /* Sequential activate */
    kcoro_activate(work->worker, kdata->testd);

    /* Busy waiting test */
    while (count--) {
        kshell_printf(kdata->ctx, "kcoro task-c running...\n");
        kshell_printf(kdata->ctx, "ktime: %lld\n", ktime_to_ns(timekeeping_get_time()));
        kshell_printf(kdata->ctx, "vtime: %lld\n", work->vtime);
        kcoro_mdelay(work, 15);
    }

    /* Normal exit */
    return -ENOERR;
}

static state work_testd(struct kcoro_work *work, void *pdata)
{
    struct kcoro_pdata *kdata = pdata;
    unsigned int count = TEST_LOOP;

    /* Block waiting test */
    while (count--) {
        kshell_printf(kdata->ctx, "kcoro task-d running...\n");
        kshell_printf(kdata->ctx, "ktime: %lld\n", ktime_to_ns(timekeeping_get_time()));
        kshell_printf(kdata->ctx, "vtime: %lld\n", work->vtime);
        kcoro_msleep(work, 10);
    }

    /* Normal exit */
    return -ENOERR;
}

static state kcoro_testing(struct kshell_context *ctx, void *pdata)
{
    struct kcoro_pdata kdata = {.ctx = ctx};
    struct kcoro_worker *worker;
    state retval;

    worker = kcoro_worker_create();
    if (unlikely(IS_INVAL(worker)))
        return PTR_INVAL(worker);

    if ((retval = PTR_INVAL_ZERO(kdata.testa = kcoro_work_create(worker, work_testa, &kdata))) ||
        (retval = PTR_INVAL_ZERO(kdata.testb = kcoro_work_create(worker, work_testb, &kdata))) ||
        (retval = PTR_INVAL_ZERO(kdata.testc = kcoro_work_create(worker, work_testc, &kdata))) ||
        (retval = PTR_INVAL_ZERO(kdata.testd = kcoro_work_create(worker, work_testd, &kdata))))
        return retval;

    kcoro_activate(worker, kdata.testa);
    kcoro_schedule(worker);
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
