/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <bitops.h>
#include <string.h>
#include <selftest.h>
#include <printk.h>

static state bitops_test_testing(void *pdata)
{
    unsigned long *bitops_tests = pdata;
    unsigned int count;

    for (count = 0; count < BITS_PER_LONG; ++count)
        bit_set(&bitops_tests[count], count);

    for (count = 0; count < BITS_PER_LONG; ++count) {
        unsigned int bit;

        printk("bitops 'ffsuf' test%u: ", count);
        if (count != (bit = ffsuf(bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);

        printk("bitops 'ffs' test%u: ", count);
        if (count + 1 != (bit = ffs(bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);

        printk("bitops 'ffzuf' test%u: ", count);
        if (count != (bit = ffzuf(~bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);

        printk("bitops 'ffz' test%u: ", count);
        if (count + 1 != (bit = ffz(~bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);

        printk("bitops 'flsuf' test%u: ", count);
        if (count != (bit = flsuf(bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);

        printk("bitops 'fls' test%u: ", count);
        if (count + 1 != (bit = fls(bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);

        printk("bitops 'flzuf' test%u: ", count);
        if (count != (bit = flzuf(~bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);

        printk("bitops 'flz' test%u: ", count);
        if (count + 1 != (bit = flz(~bitops_tests[count]))) {
            printk("failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        printk("passed (%#08lx->%u)\n", bitops_tests[count], bit);
    }

    for (count = 0; count < BITS_PER_LONG; ++count)
        bit_clr(bitops_tests, count * BITS_PER_LONG + count);

    if (memdiff(bitops_tests, 0, BYTES_PER_LONG)) {
        printk("bitops 'bit_clr' test failed\n");
        return -EFAULT;
    }

    for (count = 0; count < (BITS_PER_LONG * BITS_PER_LONG); ++count) {
        unsigned int bit;

        bit_set(bitops_tests, count);

        printk("bitops 'find_first_bit' test%u: ", count);
        bit = find_first_bit(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            printk("failed (%u)\n", bit);
            return -EFAULT;
        }
        printk("passed (%u)\n", bit);

        printk("bitops 'find_last_bit' test%u: ", count);
        bit = find_last_bit(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            printk("failed (%u)\n", bit);
            return -EFAULT;
        }
        printk("passed (%u)\n", bit);

        bit_clr(bitops_tests, count);
    }

    memset(bitops_tests, 0xff, BITS_PER_LONG * BYTES_PER_LONG);

    for (count = 0; count < (BITS_PER_LONG * BITS_PER_LONG); ++count) {
        unsigned int bit;

        bit_clr(bitops_tests, count);

        printk("bitops 'find_first_zero' test%u: ", count);
        bit = find_first_zero(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            printk("failed (%u)\n", bit);
            return -EFAULT;
        }
        printk("passed (%u)\n", bit);

        printk("bitops 'find_last_zero' test%u: ", count);
        bit = find_last_zero(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            printk("failed (%u)\n", bit);
            return -EFAULT;
        }
        printk("passed (%u)\n", bit);

        bit_set(bitops_tests, count);
    }

    return -ENOERR;
}

static void *bitops_test_prepare(int argc, char *argv[])
{
    return kzalloc(BITS_PER_LONG * BYTES_PER_LONG, GFP_KERNEL);
}

static void bitops_test_release(void *pdata)
{
    kfree(pdata);
}

static struct selftest_command bitops_command = {
    .group = "lib",
    .name = "bitops",
    .desc = "bitops unit test",
    .testing = bitops_test_testing,
    .prepare = bitops_test_prepare,
    .release = bitops_test_release,
};

static state selftest_bitops_init(void)
{
    return selftest_register(&bitops_command);
}
kshell_initcall(selftest_bitops_init);
