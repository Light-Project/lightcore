/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>
#include <driver/clk/suniv.h>
#include <driver/gpio/sunxi.h>
#include <driver/spi/suniv.h>

#include <asm/io.h>
#include <asm/proc.h>

void spi_sel(bool sel)
{
    uint32_t val = readl(SPI_BASE + SUNIV_SPI_TCR);
    val &= ~(SUNIV_SPI_TCR_SS_SEL | SUNIV_SPI_TCR_SS_VAL);
    val |= (!sel) << 7;
    writel(SPI_BASE + SUNIV_SPI_TCR, val);
}

void spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len)
{
    uint32_t xfer, count, val;

    for (xfer = min(len, 64U); len; len -= xfer) {
        writel(SPI_BASE + SUNIV_SPI_MBC, xfer);
        writel(SPI_BASE + SUNIV_SPI_MTC, xfer);
        writel(SPI_BASE + SUNIV_SPI_BCC, xfer);

        for (count = xfer; count; --count) {
            if (txbuf)
                writeb(SPI_BASE + SUNIV_SPI_TXD, *txbuf++);
            else
                writeb(SPI_BASE + SUNIV_SPI_TXD, 0xff);
        }

        val = readl(SPI_BASE + SUNIV_SPI_TCR);
        val |= SUNIV_SPI_TCR_XCH;
        writel(SPI_BASE + SUNIV_SPI_TCR, val);

        while ((readl(SPI_BASE + SUNIV_SPI_FSR) & SUNIV_SPI_FSR_RF_CNT) < xfer)
            cpu_relax();

        for (count = xfer; count; --count) {
            val = readb(SPI_BASE + SUNIV_SPI_RXD);
            if (rxbuf)
                *rxbuf++ = val;
        }
    }
}

void spi_init(uint32_t freq)
{
    uint32_t clk = ((clock_ahb / (freq * 2)) - 1);
    uint32_t val;

    /* SPI0: PC0 ~ PC3 */
    val = readl(PIO_C + SUNXI_GPIO_CFG0);
    val &= ~((0x0f<<12)|(0x0f<<8)|(0x0f<<4)|(0x0f<<0));
    val |= (0x02<<12)|(0x02<<8)|(0x02<<4)|(0x02<<0);
    writel(PIO_C + SUNXI_GPIO_CFG0, val);

    /* Reset and enable spi controller */
    val = readl(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0);
    val |= SUNIV_BUS0_SPI0;
    writel(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0, val);

    val = readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0);
    val &= ~SUNIV_BUS0_SPI0;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);
    val |= SUNIV_BUS0_SPI0;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);

    mdelay(10);

    val = readl(SPI_BASE + SUNIV_SPI_GCR);
    val |= SUNIV_SPI_GCR_SRST | SUNIV_SPI_GCR_TPEN | SUNIV_SPI_GCR_MODE | SUNIV_SPI_GCR_EN;
    writel(SPI_BASE + SUNIV_SPI_GCR, val);

    while(readl(SPI_BASE + SUNIV_SPI_GCR) & SUNIV_SPI_GCR_SRST)
        cpu_relax();

    val = readl(SPI_BASE + SUNIV_SPI_TCR);
    val |=  SUNIV_SPI_TCR_SS_OWN;
    writel(SPI_BASE + SUNIV_SPI_TCR, val);

    val = readl(SPI_BASE + SUNIV_SPI_FCR);
    val |= SUNIV_SPI_FCR_TX_FIFO_RST | SUNIV_SPI_FCR_RX_FIFO_RST;
    writel(SPI_BASE + SUNIV_SPI_FCR, val);

    writel(SPI_BASE + SUNIV_SPI_CCR, SUNIV_SPI_CCR_DRS | (clk & SUNIV_SPI_CCR_CDR2));
}

void spi_deinit(void)
{
    uint32_t val;

    val = readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0);
    val &= ~(1 << 20);
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);
}
