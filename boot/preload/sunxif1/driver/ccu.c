#include <boot.h>
#include <chip/sunxif1.h>

void ccu_cpu(uint32_t clk)
{
    int p = 0;
    int k = 1;
    int m = 1;
    uint32_t val;

    if(clk > 1152000000)
    {
        k = 2;
    }
    else if(clk > 768000000)
    {
        k = 3;
        m = 2;
    }
    
    /* Deinit CPU_CLK_SRC_REG */
    CHIP_CCU->CPU_CLK_SRC = 0x01<<16;
    
    /* Set CPU_PLL */
    val = (0x1 << 31);                                          // enable cpu pll
    val |= ((p & 0x3) << 16);                                   // set pll factor_p
    val |= ((((clk / (24000000 * k / m)) - 1) & 0x1f) << 8);    // set pll factor_n
    val |= (((k - 1) & 0x3) << 4);                              // set pll factor_k
    val |= (((m - 1) & 0x3) << 0);                              // set pll factor_m
    CHIP_CCU->PLL_CPU_CTRL = val;                               // apply changes
    while(((CHIP_CCU->PLL_CPU_CTRL>>28)&0x01)!=1);              // wait pll stable
        
    /* Set CPU_CLK_SRC_REG
     *  CPU_CLK_SRC = PLL_CPU
     */
    CHIP_CCU->CPU_CLK_SRC = (2<<16)|(1<<8)|(2<<0);
}

void ccu_sys(uint32_t clk)
{
    uint32_t k = 1;
    uint32_t m = 1;
    uint32_t val;
    
    if(clk > 768000000)
    {
        k = 2;
    }
    
    /* Deinit AHB_APB_HCLKC_CFG_REG */
    CHIP_CCU->AHB_APB_HCLKC_CFG = (1<<16)|(1<<12)|(0<<8)|(0<<6)|(1<<4);
    
    /* Set periph_pll */
    val = 1 << 31;                                          // enable periph0 pll
    val |= 1 << 18;
    val |= (((clk / (24000000 * k / m)) -1) & 0x1f)<< 8;    // set pll factor_n
    val |= ((k - 1) & 0x03) << 4;                           // set pll factor_k
    val |= ((m - 1) & 0x03) << 0;                           // set pll factor_m
    CHIP_CCU->PLL_PERIPH_CTRL = val;                        // apply changes
    while(((CHIP_CCU->PLL_PERIPH_CTRL>>28)&0x01)!=1);       // wait pll stable
    
    /* Set AHB APB CLK
     *  HCLKC_DIV = CPUCLK/1
     *  AHB_CLK_SRC = PLL_PERIPH/AHB_DIV 
     *  APB_CLK = AHB_CLK/2
     *  AHB_CLK = AHB_CLK_SRC/1
     *  AHB_RATIO_CLK = AHB_CLK_SRC/1
     */
    CHIP_CCU->AHB_APB_HCLKC_CFG = (0<<16)|(3<<12)|(0<<8)|(0<<6)|(0<<4);
    CHIP_CCU->DRAM_GATING |= (0x1 << 26) | (0x1 << 24);
}
