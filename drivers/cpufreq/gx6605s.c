/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-cpufreq"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/cpufreq.h>
#include <driver/platform/gx6605s.h>

static struct cpufreq_ops gx6605s_cpufreq_ops = {

};

static state gx6605s_cpufreq_probe(struct platform_device *pdev, void *pdata)
{
    return -ENOERR;
}

static struct platform_device_id gx6605s_cpufreq_ids[] = {
    { .name = "nationalchip,gx6605s-cpufreq" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_cpufreq_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = gx6605s_cpufreq_ids,
    .probe = gx6605s_cpufreq_probe,
};

static state gx6605s_cpufreq_init(void)
{
    return platform_driver_register(&gx6605s_cpufreq_driver);
}
driver_initcall(gx6605s_cpufreq_init);
