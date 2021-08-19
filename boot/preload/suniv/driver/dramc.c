/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Base on xboot: src/arch/arm32/mach-f1c100s/sys-dram.c
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 */

#include <boot.h>
#include <size.h>
#include <driver/clk/suniv.h>

#include <asm/io.h>
#include <asm/proc.h>

#define DRAM_SCONR          (0x00)
#define DRAM_STMG0R         (0x04)
#define DRAM_STMG1R         (0x08)
#define DRAM_SCTLR          (0x0c)
#define DRAM_SREFR          (0x10)
#define DRAM_SEXTMR         (0x14)
#define DRAM_DDLYR          (0x24)
#define DRAM_DADRR          (0x28)
#define DRAM_DVALR          (0x2c)
#define DRAM_DRPTR0         (0x30)
#define DRAM_DRPTR1         (0x34)
#define DRAM_DRPTR2         (0x38)
#define DRAM_DRPTR3         (0x3c)
#define DRAM_SEFR           (0x40)
#define DRAM_MAE            (0x44)
#define DRAM_ASPR           (0x48)
#define DRAM_SDLY0          (0x4C)
#define DRAM_SDLY1          (0x50)
#define DRAM_SDLY2          (0x54)
#define DRAM_MCR0           (0x100)
#define DRAM_MCR1           (0x104)
#define DRAM_MCR2           (0x108)
#define DRAM_MCR3           (0x10c)
#define DRAM_MCR4           (0x110)
#define DRAM_MCR5           (0x114)
#define DRAM_MCR6           (0x118)
#define DRAM_MCR7           (0x11c)
#define DRAM_MCR8           (0x120)
#define DRAM_MCR9           (0x124)
#define DRAM_MCR10          (0x128)
#define DRAM_MCR11          (0x12c)
#define DRAM_BWCR           (0x140)

#define SDR_T_CAS           (0x2)
#define SDR_T_RAS           (0x8)
#define SDR_T_RCD           (0x3)
#define SDR_T_RP            (0x3)
#define SDR_T_WR            (0x3)
#define SDR_T_RFC           (0xd)
#define SDR_T_XSR           (0xf9)
#define SDR_T_RC            (0xb)
#define SDR_T_INIT          (0x8)
#define SDR_T_INIT_REF      (0x7)
#define SDR_T_WTR           (0x2)
#define SDR_T_RRD           (0x2)
#define SDR_T_XP            (0x0)

enum dram_type {
    DRAM_TYPE_SDR    = 0,
    DRAM_TYPE_DDR    = 1,
    DRAM_TYPE_MDDR    = 2,
};

struct dram_para_t {
    uint32_t size;          // dram size (unit: MByte)
    uint32_t clk;           // dram work clock (unit: MHz)
    uint32_t access_mode;   // 0: interleave mode 1: sequence mode
    uint32_t cs_num;        // dram chip count  1: one chip  2: two chip
    uint32_t ddr8_remap;    // for 8bits data width DDR 0: normal  1: 8bits
    enum dram_type sdr_ddr; // dram type
    uint32_t bwidth;        // dram bus width
    uint32_t col_width;     // column address width
    uint32_t row_width;     // row address width
    uint32_t bank_size;     // dram bank count
    uint32_t cas;           // dram cas
};

static inline void sdelay(int loops)
{
    __asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
        "bne 1b":"=r" (loops):"0"(loops));
}

static void dram_delay(int ms)
{
    sdelay(ms * 2 * 1000);
}

static int dram_initial(void)
{
    unsigned int time = 0xffffff;

    writel(DRAMC_BASE + DRAM_SCTLR, readl(DRAMC_BASE + DRAM_SCTLR) | 0x1);
    while ((readl(DRAMC_BASE + DRAM_SCTLR) & 0x1) && time--) {
        if (time == 0)
            return 0;
    }

    return 1;
}

static int dram_delay_scan(void)
{
    unsigned int time = 0xffffff;

    writel(DRAMC_BASE + DRAM_DDLYR, readl(DRAMC_BASE + DRAM_DDLYR) | 0x1);
    while ((readl(DRAMC_BASE + DRAM_DDLYR) & 0x1) && time--) {
        if (time == 0)
            return 0;
    }

    return 1;
}

static void dram_set_autofresh_cycle(uint32_t clk)
{
    uint32_t val = 0;
    uint32_t row = 0;
    uint32_t temp = 0;

    row = readl(DRAMC_BASE + DRAM_SCONR);
    row &= 0x1e0;
    row >>= 0x5;

    if (row == 0xc) {
        if (clk >= 1000000) {
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while(temp >= (10000000 >> 6))
            {
                temp -= (10000000 >> 6);
                val++;
            }
        } else {
            val = (clk * 499) >> 6;
        }
    } else if (row == 0xb) {
        if (clk >= 1000000) {
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while (temp >= (10000000 >> 7)) {
                temp -= (10000000 >> 7);
                val++;
            }
        }
        else
            val = (clk * 499) >> 5;
    }
    writel(DRAMC_BASE + DRAM_SREFR, val);
}

static int dram_para_setup(struct dram_para_t * para)
{
    uint32_t val = 0;

    val = (para->ddr8_remap) |
        (0x1 << 1) |
        ((para->bank_size >> 2) << 3) |
        ((para->cs_num >> 1) << 4) |
        ((para->row_width - 1) << 5) |
        ((para->col_width - 1) << 9) |
        ((para->sdr_ddr ? (para->bwidth >> 4) : (para->bwidth >> 5)) << 13) |
        (para->access_mode << 15) |
        (para->sdr_ddr << 16);

    writel(DRAMC_BASE + DRAM_SCONR, val);
    writel(DRAMC_BASE + DRAM_SCTLR, readl(DRAMC_BASE + DRAM_SCTLR) | (0x1 << 19));
    return dram_initial();
}

static uint32_t dram_check_delay(uint32_t bwidth)
{
    uint32_t dsize;
    uint32_t i,j;
    uint32_t num = 0;
    uint32_t dflag = 0;

    dsize = ((bwidth == 16) ? 4 : 2);

    for (i = 0; i < dsize; i++) {
        if (i == 0)
            dflag = readl(DRAMC_BASE + DRAM_DRPTR0);
        else if (i == 1)
            dflag = readl(DRAMC_BASE + DRAM_DRPTR1);
        else if (i == 2)
            dflag = readl(DRAMC_BASE + DRAM_DRPTR2);
        else if (i == 3)
            dflag = readl(DRAMC_BASE + DRAM_DRPTR3);

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
    uint32_t k = 0;

    for (k = 0; k < 32; k++) {
        writel((void *)0x80000000 + 4 * k, k);
    }

    for (k = 0; k < 32; k++) {
        if (readl((void *)0x80000000 + 4 * k) != k)
            return 0;
    }

    return 1;
}

static uint32_t sdr_readpipe_select(void)
{
    uint32_t value = 0;
    uint32_t i = 0;

    for (i = 0; i < 8; i++) {
        writel(DRAMC_BASE + DRAM_SCTLR, (readl(DRAMC_BASE + DRAM_SCTLR) & (~(0x7 << 6))) | (i << 6));
        if (sdr_readpipe_scan()) {
            value = i;
            return value;
        }
    }

    return value;
}

static uint32_t dram_check_type(struct dram_para_t * para)
{
    uint32_t val = 0;
    uint32_t times = 0;
    uint32_t i;

    for (i = 0; i < 8; i++) {
        val = readl(DRAMC_BASE + DRAM_SCTLR);
        val &= ~(0x7 << 6);
        val |= (i << 6);
        writel(DRAMC_BASE + DRAM_SCTLR, val);

        dram_delay_scan();
        if (readl(DRAMC_BASE + DRAM_DDLYR) & 0x30)
            times++;
    }

    if (times == 8) {
        para->sdr_ddr = DRAM_TYPE_SDR;
        return 0;
    } else {
        para->sdr_ddr = DRAM_TYPE_DDR;
        return 1;
    }
}

static uint32_t dram_scan_readpipe(struct dram_para_t * para)
{
    uint32_t i, rp_best = 0, rp_val = 0;
    uint32_t val = 0;
    uint32_t readpipe[8];

    if (para->sdr_ddr == DRAM_TYPE_DDR) {
        for (i = 0; i < 8; i++) {
            val = readl(DRAMC_BASE + DRAM_SCTLR);
            val &= ~(0x7 << 6);
            val |= (i << 6);
            writel(DRAMC_BASE + DRAM_SCTLR, val);
            dram_delay_scan();
            readpipe[i] = 0;
            if ((((readl(DRAMC_BASE + DRAM_DDLYR) >> 4) & 0x3) == 0x0) &&
                (((readl(DRAMC_BASE + DRAM_DDLYR) >> 4) & 0x1) == 0x0)) {
                readpipe[i] = dram_check_delay(para->bwidth);
            }
            if (rp_val < readpipe[i]) {
                rp_val = readpipe[i];
                rp_best = i;
            }
        }
        val = readl(DRAMC_BASE + DRAM_SCTLR);
        val &= ~(0x7 << 6);
        val |= (rp_best << 6);
        writel(DRAMC_BASE + DRAM_SCTLR, val);
        dram_delay_scan();
    } else {
        val = readl(DRAMC_BASE + DRAM_SCONR);
        val &= (~(0x1 << 16));
        val &= (~(0x3 << 13));
        writel(DRAMC_BASE + DRAM_SCONR, val);
        rp_best = sdr_readpipe_select();
        val = readl(DRAMC_BASE + DRAM_SCTLR);
        val &= ~(0x7 << 6);
        val |= (rp_best << 6);
        writel(DRAMC_BASE + DRAM_SCTLR, val);
    }

    return 0;
}

static uint32_t dram_get_dram_size(struct dram_para_t * para)
{
    uint32_t colflag = 10, rowflag = 13;
    uint32_t i = 0;
    uint32_t val1 = 0;
    uint32_t count = 0;
    uint32_t addr1, addr2;

    para->col_width = colflag;
    para->row_width = rowflag;
    dram_para_setup(para);
    dram_scan_readpipe(para);

    for (i = 0; i < 32; i++) {
        *((uint32_t *)(0x80000200 + i)) = 0x11111111;
        *((uint32_t *)(0x80000600 + i)) = 0x22222222;
    }

    for (i = 0; i < 32; i++) {
        val1 = *((uint32_t *)(0x80000200 + i));
        if (val1 == 0x22222222)
            count++;
    }

    if (count == 32)
        colflag = 9;
    else
        colflag = 10;
    count = 0;
    para->col_width = colflag;
    para->row_width = rowflag;
    dram_para_setup(para);

    if (colflag == 10) {
        addr1 = 0x80400000;
        addr2 = 0x80c00000;
    } else {
        addr1 = 0x80200000;
        addr2 = 0x80600000;
    }

    for (i = 0; i < 32; i++) {
        *((uint32_t *)(addr1 + i)) = 0x33333333;
        *((uint32_t *)(addr2 + i)) = 0x44444444;
    }

    for (i = 0; i < 32; i++) {
        val1 = *((uint32_t *)(addr1 + i));
        if (val1 == 0x44444444)
            count++;
    }

    if (count == 32)
        rowflag = 12;
    else
        rowflag = 13;

    para->col_width = colflag;
    para->row_width = rowflag;

    if (para->row_width != 13)
        para->size = 16;
    else if (para->col_width == 10)
        para->size = 64;
    else
        para->size = 32;

    dram_set_autofresh_cycle(para->clk);
    para->access_mode = 0;
    dram_para_setup(para);

    return 0;
}

static void dram_setup(struct dram_para_t *para)
{
    uint32_t val = 0;
    uint32_t i;

    writel((void *)0x01c20800 + 0x24, readl((void *)0x01c20800 + 0x24) | (0x7 << 12));
    dram_delay(5);

    if (((para->cas) >> 3) & 0x1)
        writel((void *)0x01c20800 + 0x2c4, readl((void *)0x01c20800 + 0x2c4) | (0x1 << 23) | (0x20 << 17));

    if ((para->clk >= 144) && (para->clk <= 180))
        writel((void *)0x01c20800 + 0x2c0, 0xaaa);

    if (para->clk >= 180)
        writel((void *)0x01c20800 + 0x2c0, 0xfff);

    if ((para->clk) <= 96)
        val = (0x1 << 0) | (0x0 << 4) | (((para->clk * 2) / 12 - 1) << 8) | (0x1u << 31);
    else
        val = (0x0 << 0) | (0x0 << 4) | (((para->clk * 2) / 24 - 1) << 8) | (0x1u << 31);

    if (para->cas & (0x1 << 4))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xd1303333);
    else if (para->cas & (0x1 << 5))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xcce06666);
    else if (para->cas & (0x1 << 6))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xc8909999);
    else if (para->cas & (0x1 << 7))
        writel(CCU_BASE + SUNIV_PLL_DDR_PAT_CTRL, 0xc440cccc);

    if (para->cas & (0xf << 4))
        val |= 0x1 << 24;
    writel(CCU_BASE + SUNIV_PLL_DDR_CTRL, val);
    writel(CCU_BASE + SUNIV_PLL_DDR_CTRL, readl(CCU_BASE + SUNIV_PLL_DDR_CTRL) | (0x1 << 20));

    while((readl(CCU_BASE + SUNIV_PLL_DDR_CTRL) & (1 << 28)) == 0);
    dram_delay(5);
    writel(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0, readl(CCU_BASE + SUNIV_BUS_CLK_GATING_REG0) | (0x1 << 14));
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0) & ~(0x1 << 14));

    for (i = 0; i < 10; i++)
        continue;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0, readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG0) | (0x1 << 14));

    val = readl((void *)0x01c20800 + 0x2c4);
    (para->sdr_ddr == DRAM_TYPE_DDR) ? (val |= (0x1 << 16)) : (val &= ~(0x1 << 16));
    writel((void *)0x01c20800 + 0x2c4, val);

    val = (SDR_T_CAS << 0) | (SDR_T_RAS << 3) | (SDR_T_RCD << 7) | (SDR_T_RP << 10) | (SDR_T_WR << 13) | (SDR_T_RFC << 15) | (SDR_T_XSR << 19) | (SDR_T_RC << 28);
    writel(DRAMC_BASE + DRAM_STMG0R, val);
    val = (SDR_T_INIT << 0) | (SDR_T_INIT_REF << 16) | (SDR_T_WTR << 20) | (SDR_T_RRD << 22) | (SDR_T_XP << 25);
    writel(DRAMC_BASE + DRAM_STMG1R, val);
    dram_para_setup(para);
    dram_check_type(para);

    val = readl((void *)0x01c20800 + 0x2c4);
    (para->sdr_ddr == DRAM_TYPE_DDR) ? (val |= (0x1 << 16)) : (val &= ~(0x1 << 16));
    writel((void *)0x01c20800 + 0x2c4, val);

    dram_set_autofresh_cycle(para->clk);
    dram_scan_readpipe(para);
    dram_get_dram_size(para);
}

void dram_init(uint32_t freq)
{
    struct dram_para_t para = {
        .clk = freq / 1000000,
        .access_mode = 1,
        .cs_num = 1,
        .ddr8_remap = 0,
        .sdr_ddr = DRAM_TYPE_DDR,
        .bwidth = 16,
        .col_width = 10,
        .row_width = 13,
        .bank_size = 4,
        .cas = 0x3,
    };

    dram_setup(&para);

    pr_boot("DDR size: %dMiB\n", para.size);

    if ((para.size >= 64 && memtest(DRAM_BASE + (63 * size_1Mib), size_1MiB)) ||
        (para.size >= 32 && memtest(DRAM_BASE + (31 * size_1Mib), size_1MiB)) ||
        (para.size >= 16 && memtest(DRAM_BASE + (15 * size_1Mib), size_1MiB)))
        panic("DDR init error\n");

    return;
}