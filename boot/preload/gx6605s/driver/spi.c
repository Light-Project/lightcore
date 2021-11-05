/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>
#include <driver/spi/gx6605s.h>
#include <asm/io.h>

void spi_sel(bool sel)
{
    uint32_t val = readl(SPI_BASE + GX6605S_SPI_CTRL);

    if (sel) {
        val &= ~GX6605S_SPI_CTRL_CS_SEL;
        val |= GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE;
    } else {
        val &= ~GX6605S_SPI_CTRL_CS_SEL;
        val &= ~(GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE);
    }

    writel(SPI_BASE + GX6605S_SPI_CTRL, val);
}

void spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len)
{
    uint32_t xfer, count, val;

    for (; (xfer = min(len, 4)); len -= xfer) {
        /* set transmit len */
        val = readl(SPI_BASE + GX6605S_SPI_CTRL);
        val &= ~GX6605S_SPI_CTRL_ICNT_MASK;
        val |= ((xfer - 1) << 14) & GX6605S_SPI_CTRL_ICNT_MASK;
        writel(SPI_BASE + GX6605S_SPI_CTRL, val);

        for (val = 0, count = xfer; count; --count) {
            if (txbuf)
                val |= *txbuf++ << ((count - 1) * 8);
            else
                val |= 0xff << ((count - 1) * 8);
        }

        /* start to transfer data */
        writel(SPI_BASE + GX6605S_SPI_TX_FIFO, val);
        val = readl(SPI_BASE + GX6605S_SPI_CTRL);
        val |= GX6605S_SPI_CTRL_SPGO;
        writel(SPI_BASE + GX6605S_SPI_CTRL, val);

        while (!(readl(SPI_BASE + GX6605S_SPI_STAT) & GX6605S_SPI_STAT_OPE_RDY));
        val = readl(SPI_BASE + GX6605S_SPI_CTRL);
        val &= ~GX6605S_SPI_CTRL_SPGO;
        writel(SPI_BASE + GX6605S_SPI_CTRL, val);
        val = readl(SPI_BASE + GX6605S_SPI_RX_FIFO);

        for (count = xfer; count; --count)
            *rxbuf++ = val >> ((count - 1) * 8);
    }
}

void spi_init(void)
{
    writel(SPI_BASE + GX6605S_SPI_CTRL, 0x8420c002);
}
