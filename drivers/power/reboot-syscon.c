/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "syscon-reboot"

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>


static state syscon_reboot_probe(struct platform_device *pdev)
{



    return -ENOERR;
}

static struct dt_device_id syscon_reboot_ids[] = {
    { .compatible = "syscon-reboot" },
    { }, /* NULL */
};

static struct platform_driver syscon_reboot_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = syscon_reboot_ids,
    .probe = syscon_reboot_probe,
};

static state syscon_reboot_init(void)
{
    return platform_driver_register(&syscon_reboot_driver);
}
driver_initcall(syscon_reboot_init);
