/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <uuid.h>
#include <selftest.h>

#define TEST_LOOP 10

static void uuid_print(struct kshell_context *ctx, uint8_t uuid[16], const char *prefix)
{
    unsigned int count;

    kshell_printf(ctx, "%s: ", prefix);
    for (count = 0; count < 16; ++count) {
        if (count == 4 || count == 6 ||
            count == 8 || count == 10)
            kshell_puts(ctx, "-");
        kshell_printf(ctx, "%02x", uuid[count]);
    }
    kshell_putc(ctx, '\n');
}

static state uuid_test_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int count;
    guid_t guid;
    uuid_t uuid;

    for (count = 0; count < TEST_LOOP; ++count) {
        uuid_generate(&uuid);
        uuid_print(ctx, uuid.byte, "uuid generate");
    }

    for (count = 0; count < TEST_LOOP; ++count) {
        guid_generate(&guid);
        uuid_print(ctx, guid.byte, "guid generate");
    }

    return -ENOERR;
}

static struct selftest_command uuid_test_command = {
    .group = "lib",
    .name = "uuid",
    .desc = "uuid unit test",
    .testing = uuid_test_testing,
};

static state selftest_uuid_init(void)
{
    return selftest_register(&uuid_test_command);
}
kshell_initcall(selftest_uuid_init);
