/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <sections.h>
#include <libcpio.h>
#include <selftest.h>

static state libcpio_test_testing(struct kshell_context *ctx, void *pdata)
{
    const void *cpioptr = _ld_romdisk_start;
    struct cpio_data cdata;
    intptr_t offset;

    while (!cpio_data_find(&cdata, cpioptr, ROMDISK_SIZE, "", &offset)) {
        kshell_printf(ctx, "libcpio romdisk: %s\n", cdata.name);
        cpioptr += offset;
    }

    return -ENOERR;
}

static struct selftest_command libcpio_test_command = {
    .group = "lib",
    .name = "libcpio",
    .desc = "libcpio unit test",
    .testing = libcpio_test_testing,
};

static state selftest_libcpio_init(void)
{
    return selftest_register(&libcpio_test_command);
}
kshell_initcall(selftest_libcpio_init);
