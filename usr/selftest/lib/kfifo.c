/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kfifo.h>
#include <string.h>
#include <kmalloc.h>
#include <selftest.h>

#define TEST_LOOP 16

struct kfifo_test_pdata {
    DECLARE_KFIFO(normal_bytetest, char, TEST_LOOP);
    DECLARE_KFIFO(normal_longtest, long, TEST_LOOP);
    DECLARE_KFIFO_RECORD(record_bytetest, char, TEST_LOOP, 1);
    DECLARE_KFIFO_RECORD(record_longtest, long, TEST_LOOP, 1);
    DECLARE_KFIFO_DYNAMIC(dynamic_bytetest, char);
    DECLARE_KFIFO_DYNAMIC(dynamic_longtest, long);
    DECLARE_KFIFO_DYNAMIC_RECORD(dynamic_record_bytetest, char, 1);
    DECLARE_KFIFO_DYNAMIC_RECORD(dynamic_record_longtest, long, 1);
};

static const char bytetest_table[TEST_LOOP] = {
    'l', 'i', 'g', 'h', 't', 'c', 'o', 'r',
    'e', ',', 'h', 'e', 'l', 'l', 'l', 'o',
};

static const long longtest_table[TEST_LOOP] = {
    (long)0x0000000000000000ULL, (long)0x1111111111111111ULL,
    (long)0x2222222222222222ULL, (long)0x3333333333333333ULL,
    (long)0x4444444444444444ULL, (long)0x5555555555555555ULL,
    (long)0x6666666666666666ULL, (long)0x7777777777777777ULL,
    (long)0x8888888888888888ULL, (long)0x9999999999999999ULL,
    (long)0xaaaaaaaaaaaaaaaaULL, (long)0xbbbbbbbbbbbbbbbbULL,
    (long)0xccccccccccccccccULL, (long)0xddddddddddddddddULL,
    (long)0xeeeeeeeeeeeeeeeeULL, (long)0xffffffffffffffffULL,
};

static state kfifo_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct kfifo_test_pdata *kdata = pdata;
    char bytevalue[TEST_LOOP];
    long longvalue[TEST_LOOP];
    unsigned int count;
    unsigned long ret;

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_put(&kdata->normal_bytetest, bytetest_table[count]);
        kshell_printf(ctx, "kfifo normal_bytetest %u put '%c': ", count, bytetest_table[count]);
        if (!ret || kfifo_len(&kdata->normal_bytetest) != count + 1) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo normal_bytetest check full: ");
    if (!kfifo_check_full(&kdata->normal_bytetest)) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_get(&kdata->normal_bytetest, bytevalue);
        kshell_printf(ctx, "kfifo normal_bytetest %u get '%c': ", count, *bytevalue);
        if (!ret || *bytevalue != bytetest_table[count]) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo normal_bytetest copy in: ");
    ret = kfifo_in(&kdata->normal_bytetest, bytetest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo normal_bytetest copy out: ");
    ret = kfifo_out(&kdata->normal_bytetest, bytevalue, TEST_LOOP);
    if (ret != TEST_LOOP) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo normal_bytetest check copy: ");
    if (memcmp(bytevalue, bytetest_table, sizeof(bytetest_table))) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_put(&kdata->normal_longtest, longtest_table[count]);
        kshell_printf(ctx, "kfifo normal_longtest %u put %#lx: ", count, longtest_table[count]);
        if (kfifo_len(&kdata->normal_longtest) != count + 1) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo normal_longtest check full: ");
    if (!kfifo_check_full(&kdata->normal_longtest)) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_get(&kdata->normal_longtest, longvalue);
        kshell_printf(ctx, "kfifo normal_longtest %u get %#lx: ", count, *longvalue);
        if (*longvalue != longtest_table[count]) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo normal_longtest copy in: ");
    ret = kfifo_in(&kdata->normal_longtest, longtest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo normal_longtest copy out: ");
    ret = kfifo_out(&kdata->normal_longtest, longvalue, TEST_LOOP);
    if (ret != TEST_LOOP) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo normal_longtest check copy: ");
    if (memcmp(longvalue, longtest_table, sizeof(longtest_table))) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_put(&kdata->dynamic_bytetest, bytetest_table[count]);
        kshell_printf(ctx, "kfifo dynamic_bytetest %u put '%c': ", count, bytetest_table[count]);
        if (!ret || kfifo_len(&kdata->dynamic_bytetest) != count + 1) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo dynamic_bytetest check full: ");
    if (!kfifo_check_full(&kdata->dynamic_bytetest)) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_get(&kdata->dynamic_bytetest, bytevalue);
        kshell_printf(ctx, "kfifo dynamic_bytetest %u get '%c': ", count, *bytevalue);
        if (!ret || *bytevalue != bytetest_table[count]) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo dynamic_bytetest copy in: ");
    ret = kfifo_in(&kdata->dynamic_bytetest, bytetest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo dynamic_bytetest copy out: ");
    ret = kfifo_out(&kdata->dynamic_bytetest, bytevalue, TEST_LOOP);
    if (ret != TEST_LOOP)
        return -EFAULT;
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo dynamic_bytetest check copy: ");
    if (memcmp(bytevalue, bytetest_table, sizeof(bytetest_table))) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_put(&kdata->dynamic_longtest, longtest_table[count]);
        kshell_printf(ctx, "kfifo dynamic_longtest %u put %#lx: ", count, longtest_table[count]);
        if (kfifo_len(&kdata->dynamic_longtest) != count + 1) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo dynamic_longtest check full: ");
    if (!kfifo_check_full(&kdata->dynamic_longtest)) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_get(&kdata->dynamic_longtest, longvalue);
        kshell_printf(ctx, "kfifo dynamic_longtest %u get %#lx: ", count, *longvalue);
        if (*longvalue != longtest_table[count]) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    kshell_printf(ctx, "kfifo dynamic_longtest copy in: ");
    ret = kfifo_in(&kdata->dynamic_longtest, longtest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo dynamic_longtest copy out: ");
    ret = kfifo_out(&kdata->dynamic_longtest, longvalue, TEST_LOOP);
    if (ret != TEST_LOOP) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    kshell_printf(ctx, "kfifo normal_longtest check copy: ");
    if (memcmp(longvalue, longtest_table, sizeof(longtest_table))) {
        kshell_printf(ctx, "failed\n");
        return -EFAULT;
    }
    kshell_printf(ctx, "pass\n");

    for (count = 1; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "kfifo record_bytetest copy %u in: ", count);
        ret = kfifo_in(&kdata->record_bytetest, bytetest_table, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo record_bytetest copy %u out: ", count);
        ret = kfifo_out(&kdata->record_bytetest, bytevalue, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo record_bytetest check %u copy: ", count);
        if (memcmp(bytevalue, bytetest_table, count)) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    for (count = 1; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "kfifo record_longtest copy %u in: ", count);
        ret = kfifo_in(&kdata->record_longtest, longtest_table, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo record_longtest copy %u out: ", count);
        ret = kfifo_out(&kdata->record_longtest, longvalue, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo record_longtest check %u copy: ", count);
        if (memcmp(longvalue, longtest_table, count)) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    for (count = 1; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "kfifo dynamic_record_bytetest copy %u in: ", count);
        ret = kfifo_in(&kdata->dynamic_record_bytetest, bytetest_table, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo dynamic_record_bytetest copy %u out: ", count);
        ret = kfifo_out(&kdata->dynamic_record_bytetest, bytevalue, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo dynamic_record_bytetest check %u copy: ", count);
        if (memcmp(bytevalue, bytetest_table, count)) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    for (count = 1; count < TEST_LOOP; ++count) {
        kshell_printf(ctx, "kfifo dynamic_record_longtest copy %u in: ", count);
        ret = kfifo_in(&kdata->dynamic_record_longtest, longtest_table, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo dynamic_record_longtest copy %u out: ", count);
        ret = kfifo_out(&kdata->dynamic_record_longtest, longvalue, count);
        if (ret != count) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");

        kshell_printf(ctx, "kfifo dynamic_record_longtest check %u copy: ", count);
        if (memcmp(longvalue, longtest_table, count)) {
            kshell_printf(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_printf(ctx, "pass\n");
    }

    return -ENOERR;
}

static void *kfifo_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kfifo_test_pdata *kdata;
    state retval;

    kdata = kmalloc(sizeof(struct kfifo_test_pdata), GFP_KERNEL);
    if (!kdata)
        return NULL;

    kdata->normal_bytetest = KFIFO_INIT(kdata->normal_bytetest);
    kdata->normal_longtest = KFIFO_INIT(kdata->normal_longtest);
    kdata->record_bytetest = KFIFO_INIT(kdata->record_bytetest);
    kdata->record_longtest = KFIFO_INIT(kdata->record_longtest);

    if ((retval = kfifo_alloc(&kdata->dynamic_bytetest, TEST_LOOP, GFP_KERNEL)) ||
        (retval = kfifo_alloc(&kdata->dynamic_longtest, TEST_LOOP, GFP_KERNEL)) ||
        (retval = kfifo_alloc(&kdata->dynamic_record_bytetest, TEST_LOOP, GFP_KERNEL)) ||
        (retval = kfifo_alloc(&kdata->dynamic_record_longtest, TEST_LOOP, GFP_KERNEL)))
        return ERR_PTR(retval);

    return kdata;
}

static void kfifo_test_release(struct kshell_context *ctx, void *pdata)
{
    struct kfifo_test_pdata *kdata = pdata;
    kfifo_free(&kdata->dynamic_bytetest);
    kfifo_free(&kdata->dynamic_longtest);
    kfifo_free(&kdata->dynamic_record_bytetest);
    kfifo_free(&kdata->dynamic_record_longtest);
    kfree(kdata);
}

static struct selftest_command kfifo_test_command = {
    .group = "lib",
    .name = "kfifo",
    .desc = "kfifo unit test",
    .testing = kfifo_test_testing,
    .prepare = kfifo_test_prepare,
    .release = kfifo_test_release,
};

static state selftest_list_init(void)
{
    return selftest_register(&kfifo_test_command);
}
kshell_initcall(selftest_list_init);
