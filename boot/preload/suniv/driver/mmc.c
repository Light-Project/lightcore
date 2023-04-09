/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/mmc.h>
#include <driver/clk/suniv.h>
#include <driver/gpio/sunxi.h>
#include <driver/mmc/sunxi.h>

#include <asm/io.h>
#include <asm/proc.h>

static inline void mmc_transmit(struct mmc_data *data)
{
    uint32_t mask = data->rw ? SUNXI_MMC_STAR_FIFO_FULL : SUNXI_MMC_STAR_FIFO_EMPTY;
    uint32_t loop = (data->blksize * data->blkcnt) >> 2;
    uint32_t val, *buff = data->buff;

    val = readl(MMC_BASE + SUNXI_MMC_GCTL);
    val |= SUNXI_MMC_GCTL_FIFO_AC;
    writel(MMC_BASE + SUNXI_MMC_GCTL, val);

    while (loop--) {
        while (readl(MMC_BASE + SUNXI_MMC_STAR) & mask)
            cpu_relax();

        if (data->rw)
            writel(MMC_BASE + SUNXI_MMC_FIFO, *buff++);
        else
            *buff++ = readl(MMC_BASE + SUNXI_MMC_FIFO);
    }
}

void mmc_send_cmd(struct mmc_cmd *cmd, struct mmc_data *data)
{
    uint32_t val;

    if (cmd->cmdidx == MMC_STOP_TRANSMISSION)
        return;

    while (readl(MMC_BASE + SUNXI_MMC_STAR) & SUNXI_MMC_STAR_CARD_BUSY)
        cpu_relax();

    val = SUNXI_MMC_CMDR_START;
    if (!cmd->cmdidx)
        val |= SUNXI_MMC_CMDR_SEND_INIT;
    if (cmd->cmdtype & MMC_RESP_CRC)
        val |= SUNXI_MMC_CMDR_CHK_RESP_CRC;
    if (cmd->cmdtype & MMC_RESP_LONG)
        val |= SUNXI_MMC_CMDR_LONG_RESP;
    if (cmd->cmdtype & MMC_RESP_PRESENT)
        val |= SUNXI_MMC_CMDR_RESP_RCV;

    if (data) {
        val |= SUNXI_MMC_CMDR_DATA_TRANS | SUNXI_MMC_CMDR_WAIT_OVER;
        if (data->rw == MMC_WRITE)
            val |= SUNXI_MMC_CMDR_TRANS_DIR;
        if (data->blkcnt > 1)
            val |= SUNXI_MMC_CMDR_AUTO_STOP;
        writel(MMC_BASE + SUNXI_MMC_BKSR, data->blksize);
        writel(MMC_BASE + SUNXI_MMC_BYCR, data->blksize * data->blkcnt);
    }

    writel(MMC_BASE + SUNXI_MMC_CAGR, cmd->cmdarg);
    writel(MMC_BASE + SUNXI_MMC_CMDR, val | cmd->cmdidx);

    if (data) {
        mmc_transmit(data);
    }

    while (!(readl(MMC_BASE + SUNXI_MMC_RISR) & SUNXI_MMC_RISR_COMMAND_DONE))
            cpu_relax();

    if (data) {
        if (data->blkcnt > 1)
            val = SUNXI_MMC_RISR_AUTO_COMMAND_DONE;
        else
            val = SUNXI_MMC_RISR_DATA_OVER;
        while (!(readl(MMC_BASE + SUNXI_MMC_RISR) & val))
            cpu_relax();
    }

    if (cmd->cmdtype & MMC_RESP_BUSY)
        while (readl(MMC_BASE + SUNXI_MMC_STAR) & SUNXI_MMC_STAR_CARD_BUSY)
            cpu_relax();

    if (cmd->cmdtype & MMC_RESP_LONG) {
        cmd->response[3] = readl(MMC_BASE + SUNXI_MMC_RESP0);
        cmd->response[2] = readl(MMC_BASE + SUNXI_MMC_RESP1);
        cmd->response[1] = readl(MMC_BASE + SUNXI_MMC_RESP2);
        cmd->response[0] = readl(MMC_BASE + SUNXI_MMC_RESP3);
    } else
        cmd->response[0] = readl(MMC_BASE + SUNXI_MMC_RESP0);

    writel(MMC_BASE + SUNXI_MMC_RISR, ~0);

    val = readl(MMC_BASE + SUNXI_MMC_GCTL);
    val |= SUNXI_MMC_GCTL_FIFO_RST;
    writel(MMC_BASE + SUNXI_MMC_GCTL, val);
}

void mmc_init(uint32_t freq)
{
    uint32_t clk, val;

    /* MMC0: PF0 ~ PF5 */
    writel(PIO_F + SUNXI_GPIO_CFG0, 0x222222);
    writel(PIO_F + SUNXI_GPIO_DRV0, 0xfff);
    writel(PIO_F + SUNXI_GPIO_PUL0, 0x555);

    /* Setup mmc clock */
    val = clock_periph / (200 * MHZ);
    writel(CCU_BASE + SUNIV_SDMMC0_CLK, (1 << 31)|(1 << 24)|(val - 1));
    clk = clock_periph / val;

    /* Reset and enable mmc controller */
    val = readl(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0);
    val |= SUNIV_BUS0_MMC0;
    writel(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0, val);

    val = readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0);
    val &= ~SUNIV_BUS0_MMC0;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);
    val |= SUNIV_BUS0_MMC0;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);

    mdelay(10);

    /* Setup mmc controller */
    writel(MMC_BASE + SUNXI_MMC_GCTL, SUNXI_MMC_GCTL_SRST);
    while(readl(MMC_BASE + SUNXI_MMC_GCTL) & SUNXI_MMC_GCTL_SRST)
        cpu_relax();

    /* Set mmc clock */
    val = (clk / (freq * 2)) - 1;
    writel(MMC_BASE + SUNXI_MMC_CKCR, SUNXI_MMC_CKCR_EN | val);
    writel(MMC_BASE + SUNXI_MMC_CMDR, SUNXI_MMC_CMDR_START |
            SUNXI_MMC_CMDR_WAIT_OVER |SUNXI_MMC_CMDR_PRG_CLK);
    while (readl(MMC_BASE + SUNXI_MMC_CMDR) & SUNXI_MMC_CMDR_START)
        cpu_relax();
}

void mmc_deinit(void)
{
    uint32_t val;

    val = readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0);
    val &= ~SUNIV_BUS0_MMC0;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);
}
