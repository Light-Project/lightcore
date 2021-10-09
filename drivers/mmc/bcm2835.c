/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/platform.h>
#include <driver/mmc.h>
#include <driver/mmc/bcm2835.h>

static state bcm2835_mmc_init(void)
{
    return platform_driver_register();
}
driver_initcall(bcm2835_mmc_init);
