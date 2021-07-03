#include <boot.h>
#include <chip/sunxih3.h>
#include <chip/sunxi/dramc.h>
#include "dramc.h"

static inline int ns_to_t(int ns)
{
	unsigned int freq = dram_clk / 2;
	return DIV_ROUND_UP(freq * ns, 1000);
}

static void mctl_phy_init(uint32_t val)
{
	DRAM_CTL0->PIR = val | PIR_INIT;
	mctl_await_completion(&DRAM_CTL0->PGSR[0], PGSR_INIT_DONE, 0x1);
}

static void mctl_set_timing_params(struct dram_para_t * para)
{
    uint8_t  tccd        = 2;
    uint8_t  tfaw        = ns_to_t(50);
    uint8_t  trrd        = max(ns_to_t(10), 4);
    uint8_t  trcd        = ns_to_t(15);
    uint8_t  trc         = ns_to_t(53);
    uint8_t  txp         = max(ns_to_t(8), 3);
    uint8_t  twtr        = max(ns_to_t(8), 4);
    uint8_t  trtp        = max(ns_to_t(8), 4);
    uint8_t  twr         = max(ns_to_t(15), 3);
    uint8_t  trp         = ns_to_t(15);
    uint8_t  tras        = ns_to_t(38);
    uint16_t trefi       = ns_to_t(7800) / 32;
    uint16_t trfc        = ns_to_t(350);
    uint8_t  tmrw        = 0;
    uint8_t  tmrd        = 4;
    uint8_t  tmod        = 12;
    uint8_t  tcke        = 3;
    uint8_t  tcksrx      = 5;
    uint8_t  tcksre      = 5;
    uint8_t  tckesr      = 4;
    uint8_t  trasmax     = 24;
    uint8_t  tcl         = 6;                              /* CL 12 */
    uint8_t  tcwl        = 4;                              /* CWL 8 */
    uint8_t  t_rdata_en  = 4;
    uint8_t  wr_latency  = 2;
    uint32_t tdinit0     = (500 * dram_clk) + 1;           /* 500us */
    uint32_t tdinit1     = (360 * dram_clk) / 1000 + 1;    /* 360ns */
    uint32_t tdinit2     = (200 * dram_clk) + 1;           /* 200us */
    uint32_t tdinit3     = (1   * dram_clk) + 1;           /* 1us */
    uint8_t  twtp        = tcwl + 2 + twr;                 /* WL + BL / 2 + tWR */
    uint8_t  twr2rd      = tcwl + 2 + twtr;                /* WL + BL / 2 + tWTR */
    uint8_t  trd2wr      = tcl + 2 + 1 - tcwl;             /* RL + BL / 2 + 2 - WL */

    DRAM_CTL0->MR[0]= 0x1c70;   /* CL=11, WR=12 */
    DRAM_CTL0->MR[1]= 0x40;
    DRAM_CTL0->MR[2]= 0x18;     /* CWL=8 */
    DRAM_CTL0->MR[3]= 0x0;

    /* Set DRAM timing */
    DRAM_CTL0->DRAMTMG[0] = DRAMTMG0_TWTP(twtp) | DRAMTMG0_TFAW(tfaw) | DRAMTMG0_TRAS_MAX(trasmax) | DRAMTMG0_TRAS(tras);
    DRAM_CTL0->DRAMTMG[1] = DRAMTMG1_TXP(txp)   | DRAMTMG1_TRTP(trtp) | DRAMTMG1_TRC(trc);
    DRAM_CTL0->DRAMTMG[2] = DRAMTMG2_TCWL(tcwl) | DRAMTMG2_TCL(tcl)   | DRAMTMG2_TRD2WR(trd2wr) | DRAMTMG2_TWR2RD(twr2rd);
    DRAM_CTL0->DRAMTMG[3] = DRAMTMG3_TMRW(tmrw) | DRAMTMG3_TMRD(tmrd) | DRAMTMG3_TMOD(tmod);
    DRAM_CTL0->DRAMTMG[4] = DRAMTMG4_TRCD(trcd) | DRAMTMG4_TCCD(tccd) | DRAMTMG4_TRRD(trrd) | DRAMTMG4_TRP(trp);
    DRAM_CTL0->DRAMTMG[5] = DRAMTMG5_TCKSRX(tcksrx) | DRAMTMG5_TCKSRE(tcksre) | DRAMTMG5_TCKESR(tckesr) | DRAMTMG5_TCKE(tcke);

    /* Set two rank timing */
    DRAM_CTL0->DRAMTMG[8]&= ~((0xff << 8) | (0xff << 0), (0x66 << 8) | (0x10 << 0));

    /* Set PHY interface timing, write latency and read latency configure */
    DRAM_CTL0->PITMG[0]= (0x2 << 24) | (t_rdata_en << 16) | (0x1 << 8) | (wr_latency << 0);

    /* Set PHY timing, PTR0-2 use default */
    DRAM_CTL0->PTR[3]= PTR3_TDINIT0(tdinit0) | PTR3_TDINIT1(tdinit1);
    DRAM_CTL0->PTR[4]= PTR4_TDINIT2(tdinit2) | PTR4_TDINIT3(tdinit3);

    /* Set refresh timing */
    DRAM_CTL0->RFSHTMG= RFSHTMG_TREFI(trefi) | RFSHTMG_TRFC(trfc);
}

static void mctl_set_bit_delays(dramc_typedef* para)
{
    int i, j;
    uint32_t val;

    DRAM_CTL0->PGCR[0] &= ~(1 << 26);
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 11; j++)
        {
            val = DXBDLR_WRITE_DELAY(para->dx_write_delays[i][j]) |
                DXBDLR_READ_DELAY(para->dx_read_delays[i][j]);
            DRAM_CTL0->DX[i].BDLR[j]= val;
        }
    }
    for(i = 0; i < 31; i++)
    {
        val = ACBDLR_WRITE_DELAY(para->ac_delays[i]);
        DRAM_CTL0->ACBDLR[i]= val;
    }

    DRAM_CTL0->PGCR[0]|= 1 << 26;
    sdelay(1);
}

static void mctl_zq_calibration(struct dram_para_t * para)
{
	uint16_t zq_val[6];
	uint32_t val;
	uint8_t zq;
	int i;

	if((SYS->VER & 0xff) == 0 &&
	    ((*((uint32_t *)SYS_BASE + 0xf0) & 0x1) == 0)
	{
		DRAM_CTL0->ZQCR &= ~(0xffff); 
        DRAM_CTL0->ZQCR |= CONFIG_DRAM_ZQ & 0xffff;

		DRAM_CTL0->PIR = PIR_CLRSR;
		mctl_phy_init(PIR_ZCAL);

		val = DRAM_CTL0->ZQDR[0];
		val &= (0x1f << 16) | (0x1f << 0);
		val |= val << 8;
		DRAM_CTL0->ZQDR[0] =  val;

		val = DRAM_CTL0->ZQDR[1];
		val &= (0x1f << 16) | (0x1f << 0);
		val |= val << 8;
		DRAM_CTL0->ZQDR[1] = val;
		DRAM_CTL0->ZQDR[2] = val;
	}
	else
	{
		DRAM_CTL0->ZQDR[2] = 0x0a0a0a0a;

		for(i = 0; i < 6; i++)
		{
			zq = (CONFIG_DRAM_ZQ >> (i * 4)) & 0xf;
			DRAM_CTL0->ZQCR = (zq << 20) | (zq << 16) | (zq << 12) | (zq << 8) | (zq << 4) | (zq << 0);

			DRAM_CTL0->PIR = PIR_CLRSR;
			mctl_phy_init(PIR_ZCAL);

			zq_val[i] = DRAM_CTL0->ZQDR[0] & 0xff;
			DRAM_CTL0->ZQDR[2] = REPEAT_BYTE(zq_val[i]);

			DRAM_CTL0->PIR = PIR_CLRSR
            ;
			mctl_phy_init(PIR_ZCAL);

			val = DRAM_CTL0->ZQDR[0] >> 24;
			zq_val[i] |= bin_to_mgray(mgray_to_bin(val) - 1) << 8;
		}

		DRAM_CTL0->ZQDR[0] = (zq_val[1] << 16) | zq_val[0];
		DRAM_CTL0->ZQDR[1] = (zq_val[3] << 16) | zq_val[2];
		DRAM_CTL0->ZQDR[2] = (zq_val[5] << 16) | zq_val[4];
	}
}

int dram_channel_init(dramc_typedef* para)
{
    int i;

    dram_set_cr(para);
    mctl_set_timing_params(para);
    mctl_set_master_priority();

    DRAM_CTL0->PGCR[0]&= ~((1 << 30) | 0x3f);
    DRAM_CTL0->PGCR[1]&= ~(1 << 24);
    DRAM_CTL0->PGCR[1]|= 1 << 26;

    DRAM_COM->PROTECT= 0x94be6fa3;
    sdelay(100);
    DRAM_CTL0->UPD2 &= ~(0xfff << 16); 
    DRAM_CTL0->UPD2 |= 0x50 << 16;
    DRAM_COM->PROTECT= 0x0;
    sdelay(100);

    for(i = 0; i < 4; i++)
        DRAM_CTL0->DX[i].GCR = (0x3 << 4) | (0x1 << 1) | (0x3 << 2) | (0x3 << 12) | (0x3 << 14), (0x0 << 4);

    DRAM_CTL0->ACIOCR |= 0x1 << 1;
    
    DRAM_CTL0->PGCR[2] &= ~(0x3 << 6);
    DRAM_CTL0->PGCR[0] |= (0x3 << 14) | (0x3 << 12);
    DRAM_CTL0->PGCR[2] &= ~((0x3 << 10) | (0x3 << 8));
    DRAM_CTL0->PGCR[2] |= (0x1 << 10) | (0x2 << 8);

    if(!para->bus_full_width)
    {
        DRAM_CTL0->DX[2].GCR = 0x0;
        DRAM_CTL0->DX[3].GCR = 0x0;
    }

    DRAM_CTL0->DTCR &= ~(0xf << 24); 
    DRAM_CTL0->DTCR |= (para->dual_rank ? 0x3 : 0x1) << 24;
    mctl_set_bit_delays(para);
    sdelay(50);

    mctl_zq_calibration(para);
    mctl_phy_init(PIR_PLLINIT | PIR_DCAL | PIR_PHYRST | PIR_DRAMRST | PIR_DRAMINIT | PIR_QSGATE);

    if(DRAM_CTL0->PGSR[0] & (0xfe << 20))
    {
        if(((DRAM_CTL0->DX[0].GSR[0] >> 24) & 0x2) ||
           ((DRAM_CTL0->DX[1].GSR[0] >> 24) & 0x2))
        {
            DRAM_CTL0->DTCR &= ~(0xf << 24);
            DRAM_CTL0->DTCR |= 0x1 << 24;
            para->dual_rank = 0;
        }

        if(((DRAM_CTL0->DX[2].GSR[0] >> 24) & 0x1) ||
            ((DRAM_CTL0->DX[3].GSR[0] >> 24) & 0x1))
        {
            DRAM_CTL0->DX[2].GCR = 0x0;
            DRAM_CTL0->DX[3].GCR = 0x0;
            para->bus_full_width = 0;
        }

        mctl_set_cr(para);
        sdelay(20);

        mctl_phy_init(PIR_QSGATE);
        if(DRAM_CTL0->PGSR[0] & (0xfe << 20))
            return 1;
    }

    mctl_await_completion(&DRAM_CTL0->STATR, 0x1, 0x1);

    DRAM_CTL0->RFSHCTL0 |= 0x1 << 31;
    sdelay(10);
    DRAM_CTL0->RFSHCTL0 &= ~(0x1 << 31);
    sdelay(10);

    DRAM_CTL0->PGCR[3]= 0x00aa0060;
    DRAM_CTL0->ZQCR|= ZQCR_PWRDOWN;
    DRAM_COM->MAER= 0xffffffff;

    return 0;
}
