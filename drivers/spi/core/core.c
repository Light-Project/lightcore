/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/spi.h>

state spi_host_register(struct spi_host *host)
{
    if (!host || !host->ops)
        return -EINVAL;

    return -ENOERR;
}


