/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kmalloc.h>
#include <bitops.h>
#include <string.h>
#include <selftest.h>

static state bitops_test_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned long *bitops_tests = pdata;
    unsigned int pass, count, from;
    unsigned int scount, sfrom;

    memset(pdata, 0, BITS_PER_LONG * BYTES_PER_LONG);

    for (count = 0; count < BITS_PER_LONG; ++count)
        bit_set(&bitops_tests[count], count);

    for (count = 0; count < BITS_PER_LONG; ++count) {
        unsigned int bit;

        kshell_printf(ctx, "bitops 'ffsuf' test%u: ", count);
        if (count != (bit = ffsuf(bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);

        kshell_printf(ctx, "bitops 'ffs' test%u: ", count);
        if (count + 1 != (bit = ffs(bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);

        kshell_printf(ctx, "bitops 'ffzuf' test%u: ", count);
        if (count != (bit = ffzuf(~bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);

        kshell_printf(ctx, "bitops 'ffz' test%u: ", count);
        if (count + 1 != (bit = ffz(~bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);

        kshell_printf(ctx, "bitops 'flsuf' test%u: ", count);
        if (count != (bit = flsuf(bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);

        kshell_printf(ctx, "bitops 'fls' test%u: ", count);
        if (count + 1 != (bit = fls(bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);

        kshell_printf(ctx, "bitops 'flzuf' test%u: ", count);
        if (count != (bit = flzuf(~bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);

        kshell_printf(ctx, "bitops 'flz' test%u: ", count);
        if (count + 1 != (bit = flz(~bitops_tests[count]))) {
            kshell_printf(ctx, "failed (%#08lx->%u)\n", bitops_tests[count], bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%#08lx->%u)\n", bitops_tests[count], bit);
    }

    count = 0;

    for_each_set_bit(from, bitops_tests, BITS_PER_LONG * BITS_PER_LONG) {
        kshell_printf(ctx, "bitops 'for_each_set_bit' test%u: ", count);
        pass = count * BITS_PER_LONG + count;
        if (pass != from) {
            kshell_printf(ctx, "failed (%u->%u)\n", pass, from);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u->%u)\n", pass, from);
        if (count >= BITS_PER_LONG / 2)
            break;
        count++;
    }

    scount = count++;
    sfrom = from;

    for_each_set_bit_continue(from, bitops_tests, BITS_PER_LONG * BITS_PER_LONG) {
        kshell_printf(ctx, "bitops 'for_each_set_bit_continue' test%u: ", count);
        pass = count * BITS_PER_LONG + count;
        if (pass != from) {
            kshell_printf(ctx, "failed (%u->%u)\n", pass, from);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u->%u)\n", pass, from);
        count++;
    }

    for_each_set_bit_from(sfrom, bitops_tests, BITS_PER_LONG * BITS_PER_LONG) {
        kshell_printf(ctx, "bitops 'for_each_set_bit_from' test%u: ", scount);
        pass = scount * BITS_PER_LONG + scount;
        if (pass != sfrom) {
            kshell_printf(ctx, "failed (%u->%u)\n", pass, sfrom);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u->%u)\n", pass, sfrom);
        scount++;
    }

    memset(pdata, 0xff, BITS_PER_LONG * BYTES_PER_LONG);

    for (count = 0; count < BITS_PER_LONG; ++count)
        bit_clr(bitops_tests, count * BITS_PER_LONG + count);

    count = 0;

    for_each_clear_bit(from, bitops_tests, BITS_PER_LONG * BITS_PER_LONG) {
        kshell_printf(ctx, "bitops 'for_each_clear_bit' test%u: ", count);
        pass = count * BITS_PER_LONG + count;
        if (pass != from) {
            kshell_printf(ctx, "failed (%u->%u)\n", pass, from);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u->%u)\n", pass, from);
        if (count >= BITS_PER_LONG / 2)
            break;
        count++;
    }

    scount = count++;
    sfrom = from;

    for_each_clear_bit_continue(from, bitops_tests, BITS_PER_LONG * BITS_PER_LONG) {
        kshell_printf(ctx, "bitops 'for_each_clear_bit_continue' test%u: ", count);
        pass = count * BITS_PER_LONG + count;
        if (pass != from) {
            kshell_printf(ctx, "failed (%u->%u)\n", pass, from);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u->%u)\n", pass, from);
        count++;
    }

    for_each_clear_bit_from(sfrom, bitops_tests, BITS_PER_LONG * BITS_PER_LONG) {
        kshell_printf(ctx, "bitops 'for_each_clear_bit_from' test%u: ", scount);
        pass = scount * BITS_PER_LONG + scount;
        if (pass != sfrom) {
            kshell_printf(ctx, "failed (%u->%u)\n", pass, sfrom);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u->%u)\n", pass, sfrom);
        scount++;
    }

    memset(pdata, 0, BITS_PER_LONG * BYTES_PER_LONG);

    for (count = 0; count < (BITS_PER_LONG * BITS_PER_LONG); ++count) {
        unsigned int bit;

        bit_set(bitops_tests, count);

        kshell_printf(ctx, "bitops 'find_first_bit' test%u: ", count);
        bit = find_first_bit(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            kshell_printf(ctx, "failed (%u)\n", bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u)\n", bit);

        kshell_printf(ctx, "bitops 'find_last_bit' test%u: ", count);
        bit = find_last_bit(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            kshell_printf(ctx, "failed (%u)\n", bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u)\n", bit);

        bit_clr(bitops_tests, count);
    }

    memset(bitops_tests, 0xff, BITS_PER_LONG * BYTES_PER_LONG);

    for (count = 0; count < (BITS_PER_LONG * BITS_PER_LONG); ++count) {
        unsigned int bit;

        bit_clr(bitops_tests, count);

        kshell_printf(ctx, "bitops 'find_first_zero' test%u: ", count);
        bit = find_first_zero(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            kshell_printf(ctx, "failed (%u)\n", bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u)\n", bit);

        kshell_printf(ctx, "bitops 'find_last_zero' test%u: ", count);
        bit = find_last_zero(bitops_tests, BITS_PER_LONG * BITS_PER_LONG);
        if (bit != count) {
            kshell_printf(ctx, "failed (%u)\n", bit);
            return -EFAULT;
        }
        kshell_printf(ctx, "passed (%u)\n", bit);

        bit_set(bitops_tests, count);
    }

    return -ENOERR;
}

static void *bitops_test_prepare(struct kshell_context *ctx, int argc, char *argv[])
{
    return kmalloc(BITS_PER_LONG * BYTES_PER_LONG, GFP_KERNEL);
}

static void bitops_test_release(struct kshell_context *ctx, void *pdata)
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
