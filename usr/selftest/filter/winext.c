/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <kmalloc.h>
#include <prandom.h>
#include <filter/winext.h>

#define TEST_LOOP 100
#define TEST_WIN 10
#define TEST_LIMIT 1000

struct winext_test {
    struct winext_state *winmin;
    struct winext_state *winmax;
};

static state winext_testing(struct kshell_context *ctx, void *pdata)
{
    struct winext_test *wtest = pdata;
    intptr_t value, winmax, winmin;
    unsigned int count;

    /* Linear incremental test */
    winext_init_min(wtest->winmin);

    for (count = 0; count < TEST_LOOP; ++count) {
        value = count;
        winmin = winext_sample_min(wtest->winmin, TEST_WIN, count, value);
        kshell_printf(ctx, "winext linear incremental test%02d: %2ld (%2ld)\n",
                      count, value, winmin);
    }

    /* Linear decline test */
    winext_init_max(wtest->winmax);

    for (count = 0; count < TEST_LOOP; ++count) {
        value = TEST_LOOP - count - 1;
        winmax = winext_sample_max(wtest->winmin, TEST_WIN, count, value);
        kshell_printf(ctx, "winext linear decline test%02d: %2ld (%2ld)\n",
                      count, value, winmax);
    }

    /* Random two-way test */
    winext_init_min(wtest->winmin);
    winext_init_max(wtest->winmax);

    for (count = 0; count < TEST_LOOP; ++count) {
        value = prandom_value() % TEST_LIMIT;
        winmin = winext_sample_min(wtest->winmin, TEST_WIN, count, value);
        winmax = winext_sample_max(wtest->winmax, TEST_WIN, count, value);
        kshell_printf(ctx, "winext random test%02d: %3ld (%3ld - %3ld)\n",
                      count, value, winmin, winmax);
    }

    return -ENOERR;
}

static void *winext_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct winext_test *wtest;

    wtest = kmalloc(sizeof(*wtest), GFP_KERNEL);
    if (!wtest)
        return NULL;

    wtest->winmin = winext_alloc(3, GFP_KERNEL);
    wtest->winmax = winext_alloc(3, GFP_KERNEL);
    if (!wtest->winmin || !wtest->winmax)
        return NULL;

    return wtest;
}

static void winext_release(struct kshell_context *ctx, void *pdata)
{
    struct winext_test *wtest = pdata;
    winext_free(wtest->winmin);
    winext_free(wtest->winmax);
    kfree(wtest);
}

static struct selftest_command winext_command = {
    .group = "filter",
    .name = "winext",
    .desc = "windowed extremum tracker test",
    .testing = winext_testing,
    .prepare = winext_prepare,
    .release = winext_release,
};

static state selftest_winext_init(void)
{
    return selftest_register(&winext_command);
}
kshell_initcall(selftest_winext_init);
