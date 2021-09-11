/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Base on xboot: src/arch/arm32/mach-f1c100s/sys-dram.c
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <size.h>
#include <driver/clk/suniv.h>
#include <driver/gpio/sunxi.h>
#include <asm/io.h>
#include <asm/proc.h>

#define DRAM_ROW1_WIDTH  13 /* Preferred Row Configuration */
#define DRAM_ROW2_WIDTH  12 /* Alternative Row Configuration */
#define DRAM_COL1_WIDTH  10 /* Preferred Column Configuration */
#define DRAM_COL2_WIDTH  9  /* Alternative Column Configuration */
#define DRAM_CAS_LATENCY 3  /* Alternative Column Configuration */
#define DRAM_DATA_WIDTH  16 /* Data Bus Width */
#define DRAM_BANK_NUM    4  /* Internal Banks Number */
#define DRAM_CS_NUM      1  /* Dram Chip Number */

struct dram_para {
    union {
        struct {
            uint32_t ddr_8bit:1;
            uint32_t enable:2;
            uint32_t bank_size:1;
            uint32_t cs_number:1;
            uint32_t row_width:4;
            uint32_t col_width:4;
            uint32_t data_width:2;
            uint32_t access_mode:1;
            uint32_t dram_type:1;
        };
        uint32_t val;
    };
} __packed;

enum dram_type {
    DRAM_TYPE_SDR   = 0,
    DRAM_TYPE_DDR   = 1,
};

enum dram_mode {
    DRAM_MODE_INT   = 0,
    DRAM_MODE_SEQ   = 1,
};

enum dram_remap {
    DRAM_REMAP_NOR  = 0,
    DRAM_REMAP_8BIT = 1,
};

static void dram_delay_scan(void)
{
    uint32_t val;

    val = readl(DRAMC_BASE + SUNIV_DRAMC_DDLY);
    val |= 0x1 << 0;
    writel(DRAMC_BASE + SUNIV_DRAMC_DDLY, val);

    while (readl(DRAMC_BASE + SUNIV_DRAMC_DDLY) & 0x1)
        cpu_relax();
}

static uint32_t dram_check_delay(uint32_t data_width)
{
    uint32_t dsize;
    uint32_t i, j;
    uint32_t num = 0;
    uint32_t dflag = 0;

    dsize = (data_width == 16) ? 4 : 2;

    for (i = 0; i < dsize; i++) {
        if (i == 0)
            dflag = readl(DRAMC_BASE + SUNIV_DRAMC_DRPTR0);
        else if (i == 1)
            dflag = readl(DRAMC_BASE + SUNIV_DRAMC_DRPTR1);
        else if (i == 2)
            dflag = readl(DRAMC_BASE + SUNIV_DRAMC_DRPTR2);
        else if (i == 3)
            dflag = readl(DRAMC_BASE + SUNIV_DRAMC_DRPTR3);

        for (j = 0; j < 32; j++) {
            if (dflag & 0x1)
                num++;
            dflag >>= 1;
        }
    }

    return num;
}

static int sdr_readpipe_scan(void)
{
    uint32_t tmp;

    for (tmp = 0; tmp < 32; tmp++)
        writel((void *)0x80000000 + 4 * tmp, tmp);

    for (tmp = 0; tmp < 32; tmp++)
        if (readl((void *)0x80000000 + 4 * tmp) != tmp)
            return 0;

    return 1;
}

static uint32_t sdr_readpipe_select(void)
{
    uint32_t val, count;

    for (count = 0; count < 8; count++) {
        val = readl(DRAMC_BASE + SUNIV_DRAMC_SCTL);
        val &= ~(0x7 << 6);
        val |= count << 6;
        writel(DRAMC_BASE + SUNIV_DRAMC_SCTL, val);

        if (sdr_readpipe_scan())
            return count;
    }

    return 0;
}

static void dram_scan_readpipe(struct dram_para *para)
{
    uint32_t i, rp_best, rp_val = 0;
    uint32_t val;
    uint32_t readpipe[8];

    if (para->dram_type == DRAM_TYPE_DDR) {
        for (i = 0; i < 8; i++) {
            val = readl(DRAMC_BASE + SUNIV_DRAMC_SCTL);
            val &= ~(0x7 << 6);
            val |= (i << 6);
            writel(DRAMC_BASE + SUNIV_DRAMC_SCTL, val);
            dram_delay_scan();
            readpipe[i] = 0;

            if (!((readl(DRAMC_BASE + SUNIV_DRAMC_DDLY) >> 4) & 0x3)) {
                readpipe[i] = dram_check_delay(para->data_width);
            }

            if (rp_val < readpipe[i]) {
                rp_val = readpipe[i];
                rp_best = i;
            }
        }

        val = readl(DRAMC_BASE + SUNIV_DRAMC_SCTL);
        val &= ~(0x7 << 6);
        val |= rp_best << 6;
        writel(DRAMC_BASE + SUNIV_DRAMC_SCTL, val);
        dram_delay_scan();
    } else if (para->dram_type == DRAM_TYPE_SDR) {
        val = readl(DRAMC_BASE + SUNIV_DRAMC_SCON);
        val &= ~(0x1 << 16);
        val &= ~(0x3 << 13);
        writel(DRAMC_BASE + SUNIV_DRAMC_SCON, val);

        val = readl(DRAMC_BASE + SUNIV_DRAMC_SCTL);
        val &= ~(0x7 << 6);
        val |= sdr_readpipe_select() << 6;
        writel(DRAMC_BASE + SUNIV_DRAMC_SCTL, val);
    }
}

static int dram_type_get(void)
{
    uint32_t val, i, times = 0;

    for (i = 0; i < 8; i++) {
        val = readl(DRAMC_BASE + SUNIV_DRAMC_SCTL);
        val &= ~(0x7 << 6);
        val |= (i << 6);
        writel(DRAMC_BASE + SUNIV_DRAMC_SCTL, val);
        dram_delay_scan();
        if (readl(DRAMC_BASE + SUNIV_DRAMC_DDLY) & 0x30)
            times++;
    }

    if (times == 8)
        return DRAM_TYPE_SDR;
    return DRAM_TYPE_DDR;
}

static void dram_para_setup(struct dram_para *para)
{
    uint32_t val;

    writel(DRAMC_BASE + SUNIV_DRAMC_SCON, para->val);

    val = readl(DRAMC_BASE + SUNIV_DRAMC_SCTL);
    val |= 0x1 << 19;
    writel(DRAMC_BASE + SUNIV_DRAMC_SCTL, val);

    val = readl(DRAMC_BASE + SUNIV_DRAMC_SCTL);
    val |= 0x1 << 0;
    writel(DRAMC_BASE + SUNIV_DRAMC_SCTL, val);

    while (readl(DRAMC_BASE + SUNIV_DRAMC_SCTL) & 0x1)
        cpu_relax();
}

static void dram_autorefresh(struct dram_para *para, uint32_t clk)
{
    uint32_t tmp, val = 0;

    if ((para->row_width + 1) == 12) {
        if (clk >= 1000000) {
            tmp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while (tmp >= (10000000 >> 7)) {
                tmp -= (10000000 >> 7);
                val++;
            }
        }
        else
            val = (clk * 499) >> 5;
    } else if ((para->row_width + 1) == 13) {
        if (clk >= 1000000) {
            tmp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while(tmp >= (10000000 >> 6)) {
                tmp -= (10000000 >> 6);
                val++;
            }
        } else
            val = (clk * 499) >> 6;
    }

    writel(DRAMC_BASE + SUNIV_DRAMC_SREF, val);
}

static uint32_t dram_autoset(struct dram_para *para, uint32_t freq)
{
    uint32_t *addr1, *addr2;
    unsigned int i, count;
    uint32_t col, row, val;

    /* Check col parameters */
    addr1 = (uint32_t *)0x80000200;
    addr2 = (uint32_t *)0x80000600;

    for (i = 0; i < 32; i++) {
        writel(addr1 + i, 0x5a5a5a5a);
        writel(addr2 + i, 0xa5a5a5a5);
    }

    for (count = 0, i = 0; i < 32; i++)
        if (readl(addr1 + i) == 0xa5a5a5a5)
            count++;

    if (count)
        col = DRAM_COL2_WIDTH;
    else
        col = DRAM_COL1_WIDTH;

    if (count) {
        para->col_width = col - 1;
        dram_para_setup(para);
    }

    /* Check row parameters */
    if (col == DRAM_COL1_WIDTH) {
        addr1 = (uint32_t *)0x80400000;
        addr2 = (uint32_t *)0x80c00000;
    } else {
        addr1 = (uint32_t *)0x80200000;
        addr2 = (uint32_t *)0x80600000;
    }

    for (i = 0; i < 32; i++) {
        writel(addr1 + i, 0x5a5a5a5a);
        writel(addr2 + i, 0xa5a5a5a5);
    }

    for (count = 0, i = 0; i < 32; i++)
        if (readl(addr1 + i) == 0xa5a5a5a5)
            count++;

    if (count)
        row = DRAM_ROW2_WIDTH;
    else
        row = DRAM_ROW1_WIDTH;

    if (count) {
        para->row_width = row - 1;
        dram_autorefresh(para, freq);
    }

    /* Turn on interleaving mode */
    para->access_mode = DRAM_MODE_INT;
    dram_para_setup(para);

    if (row == 12)
        val = 2;
    else if (col == 9)
        val = 4;
    else
        val = 8;

    val *= DRAM_DATA_WIDTH / 8;
    val *= DRAM_CS_NUM * DRAM_BANK_NUM;
    return val;
}

void dramc_init(uint32_t freq)
{
    uint32_t val;

    /* Fixed initial configuration */
    struct dram_para para = {
        .enable      = true,
        .ddr_8bit    = DRAM_REMAP_NOR,
        .access_mode = DRAM_MODE_SEQ,
        .dram_type   = DRAM_TYPE_DDR,
        .bank_size   = (DRAM_BANK_NUM >> 2),
        .cs_number   = (DRAM_CS_NUM - 1),
        .row_width   = (DRAM_ROW1_WIDTH - 1),
        .col_width   = (DRAM_COL1_WIDTH - 1),
    };

    if (para.dram_type == DRAM_TYPE_DDR)
        para.data_width = DRAM_DATA_WIDTH >> 4;
    else if ((para.dram_type == DRAM_TYPE_SDR))
        para.data_width = (DRAM_DATA_WIDTH >> 5) << 13;

    /* Disable DDR_REF_D */
    val = readl(PIO_B + SUNXI_GPIO_CFG0);
    val |= 0x7 << 12;
    writel(PIO_B + SUNXI_GPIO_CFG0, val);

    mdelay(10);

    /* Reference configuration factor */
    val = readl(PIO_BASE + SUNIV_GPIO_SDR_PUL);
    if ((DRAM_CAS_LATENCY >> 3) & 0x1)
        val |= (0x1 << 23) | (0x20 << 17);
    else
        val &= ~(0x1 << 23);
    writel(PIO_BASE + SUNIV_GPIO_SDR_PUL, val);

    /* Configure dram io level type */
    val = readl(PIO_BASE + SUNIV_GPIO_SDR_PUL);
    if (para.dram_type == DRAM_TYPE_DDR)  /* 2.5v */
        val |= 0x01 << 16;
    else  /* 3.3v */
        val &= ~(0x01 << 16);
    writel(PIO_BASE + SUNIV_GPIO_SDR_PUL, val);

    /* Configure dram drive capability */
    if ((360 * MHZ) <= freq)
        writel(PIO_BASE + SUNIV_GPIO_SDR_DRV, 0x3fff);
    else if (((288 * MHZ) <= freq) && (freq < (360 * MHZ)))
        writel(PIO_BASE + SUNIV_GPIO_SDR_DRV, 0x2aaa);
    else if (((192 * MHZ) <= freq) && (freq < (288 * MHZ)))
        writel(PIO_BASE + SUNIV_GPIO_SDR_DRV, 0x1555);
    else
        writel(PIO_BASE + SUNIV_GPIO_SDR_DRV, 0x0000);

    if (DRAM_CAS_LATENCY & (0x1 << 4))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xd1303333);
    else if (DRAM_CAS_LATENCY & (0x1 << 5))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xcce06666);
    else if (DRAM_CAS_LATENCY & (0x1 << 6))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xc8909999);
    else if (DRAM_CAS_LATENCY & (0x1 << 7))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xc440cccc);

    if (DRAM_CAS_LATENCY & (0xf << 4)) {
        val = readl(CCU_BASE + SUNIV_PLL_DDR_CTRL);
        val = 0x1 << 24;
        writel(CCU_BASE + SUNIV_PLL_DDR_CTRL, val);
    }

    mdelay(10);

    /* Reset and enable dram controller */
    val = readl(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0);
    val |= SUNIV_BUS0_SDRAM;
    writel(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0, val);

    val = readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0);
    val &= ~SUNIV_BUS0_SDRAM;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);
    mdelay(50);
    val |= SUNIV_BUS0_SDRAM;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, val);

    writel(DRAMC_BASE + SUNIV_DRAMC_STMG0,      SUNIV_DRAMC_STMG0_RC  | SUNIV_DRAMC_STMG0_XSR |
                        SUNIV_DRAMC_STMG0_RFC | SUNIV_DRAMC_STMG0_WR  | SUNIV_DRAMC_STMG0_RP  |
                        SUNIV_DRAMC_STMG0_RCD | SUNIV_DRAMC_STMG0_RAS | SUNIV_DRAMC_STMG0_CAS );
    writel(DRAMC_BASE + SUNIV_DRAMC_STMG1,      SUNIV_DRAMC_STMG1_RRD | SUNIV_DRAMC_STMG1_WTR |
                        SUNIV_DRAMC_STMG1_REF | SUNIV_DRAMC_STMG1_INIT);

    /* Get dram type */
    dram_para_setup(&para);
    para.dram_type = dram_type_get();

    /* Configure dram io level type */
    val = readl(PIO_BASE + SUNIV_GPIO_SDR_PUL);
    if (para.dram_type == DRAM_TYPE_DDR)  /* 2.5v */
        val |= 0x01 << 16;
    else  /* 3.3v */
        val &= ~(0x01 << 16);
    writel(PIO_BASE + SUNIV_GPIO_SDR_PUL, val);

    pr_boot("Dram type: %s\n", para.dram_type ? "ddr" : "sdram");

    /* Get memory size */
    dram_autorefresh(&para, freq);
    dram_scan_readpipe(&para);
    dram_para_setup(&para);
    val = dram_autoset(&para, freq);

    pr_boot("Dram size: %dMiB\n", val);
    if ((val >= 64 && memtest_fast(DRAM_BASE + (63 * size_1Mib), size_1KiB)) ||
        (val >= 32 && memtest_fast(DRAM_BASE + (31 * size_1Mib), size_1KiB)) ||
        (val >= 16 && memtest_fast(DRAM_BASE + (15 * size_1Mib), size_1KiB)))
        pr_boot("Dram init error\n");
}
