/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <math.h>
#include <selftest.h>

struct rational_template {
    unsigned int num, den;
    unsigned int maxnum, maxden;
    unsigned int expnum, expden;
    const char *name;
};

static const struct rational_template test_rational_parameters[] = {
    {
        1230, 10, 100, 20, 100, 1,
        "Exceeds bounds, semi-convergent term > 1/2 last term"
    }, {
        34567, 100, 120, 20, 120, 1,
        "Exceeds bounds, semi-convergent term < 1/2 last term"
    }, {
        1, 30, 100, 10, 0, 1,
        "Closest to zero"
    }, {
        1, 19, 100, 10, 1, 10,
        "Closest to smallest non-zero"
    }, {
        27, 32, 16, 16,  11, 13,
        "Use convergent"
    }, {
        1155, 7735, 255, 255, 33, 221,
        "Exact answer"
    }, {
        87, 32, 70, 32,  68, 25,
        "Semiconvergent, numerator limit"
    }, {
        14533, 4626, 15000, 2400, 7433, 2366,
        "Semiconvergent, denominator limit"
    },
};

static state rational_testing(struct kshell_context *ctx, void *pdata)
{
    unsigned int numer, denomin;
    unsigned int count;

    for (count = 0; count < ARRAY_SIZE(test_rational_parameters); ++count) {
        const struct rational_template *template;

        template = &test_rational_parameters[count];
        rational(template->num, template->den, template->maxnum,
                 template->maxden, &numer, &denomin);
        kshell_printf(ctx, "rational test%u: %u >> %u [%u : %u] = %u / %u (%s): ",
                      count, template->num, template->den, template->maxnum,
                      template->maxden, numer, denomin, template->name);

        if (test_rational_parameters[count].expnum != numer ||
            test_rational_parameters[count].expden != denomin) {
            kshell_puts(ctx, "failed\n");
            return -EFAULT;
        }
        kshell_puts(ctx, "passed\n");
    }

    return -ENOERR;
}

static struct selftest_command rational_command = {
    .group = "math",
    .name = "rational",
    .testing = rational_testing,
};

static state selftest_rational_init(void)
{
    return selftest_register(&rational_command);
}
kshell_initcall(selftest_rational_init);
