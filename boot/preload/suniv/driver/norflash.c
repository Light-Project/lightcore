/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>

bool norflash_id(void)
{
    uint8_t cmd[4] = {
        [0] = 0x9f,
    };

    spi_sel(true);
    spi_transmit(cmd, cmd, ARRAY_SIZE(cmd));
    spi_sel(false);

    pr_boot("NorFlash Manufacturer: 0x%x\n", cmd[1]);
    pr_boot("NorFlash Memory Type: 0x%x\n", cmd[2]);
    pr_boot("NorFlash Capacity: 0x%x\n", cmd[3]);

    return (cmd[1] == 0x00 && cmd[2] == 0x00 && cmd[3] == 0x00) ||
           (cmd[1] == 0xff && cmd[2] == 0xff && cmd[3] == 0xff);
}

void norflash_read(uint8_t *buff, size_t addr, uint32_t len)
{
    uint8_t cmd[5] = {
        [0] = 0x0b,
        [1] = addr >> 16,
        [2] = addr >> 8,
        [3] = addr >> 0,
    };

    spi_sel(true);
    spi_transmit(cmd, NULL, ARRAY_SIZE(cmd));
    spi_transmit(NULL, buff, len);
    spi_sel(false);
}