/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/cpuidle.h>

static unsigned int dummy_select(struct cpuidle_device *cdev)
{
    return 0;
}

static struct cpuidle_governor dummy_governor = {
    .name = "dummy",
    .rating = CPUIDLE_RATING_DUMMY,
    .select = dummy_select,
};

static state haltpoll_init(void)
{
    /* Dummy governor always exists and will never exit. */
    return cpuidle_governor_register(&dummy_governor);
}
postcore_initcall(haltpoll_init);
