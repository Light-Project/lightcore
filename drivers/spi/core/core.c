/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/spi.h>

state spi_transfer(struct spi_device *spi, void *txbuf, void *rxbuf, size_t len)
{

    return -ENOERR;
}

state spi_host_register(struct spi_host *host)
{
    if (!host->ops)
        return -EINVAL;

    return -ENOERR;
}

void spi_host_unregister(struct spi_host *host)
{

}
