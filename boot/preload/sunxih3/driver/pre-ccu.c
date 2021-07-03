#include <boot.h>
#include <chip/sunxih3.h>

void pre_ccu_cpu(uint32_t clk)
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
    
    CCU->CPUX_AXI_CFG = (1<<16)|(0<<8)|(0<<0);  // Set cpuclk src to the 24mOSC
    
    val = (0x1 << 31);                          // enable cpu pll
    val |= ((p & 0x3) << 16);                   // set pll factor_p
    val |= ((((clk / (24000000 * k / m)) - 1) & 0x1f) << 8); // set pll factor_n
    val |= (((k - 1) & 0x3) << 4);              // set pll factor_k
    val |= (((m - 1) & 0x3) << 0);              // set pll factor_m
    CCU->PLL_CPUX_CTRL = val;                   // apply changes
    while(((CCU->PLL_CPUX_CTRL>>28)&0x01)!=1);  // wait pll stable
    
    CCU->CPUX_AXI_CFG = (2<<16)|(1<<8)|(2<<0);  // Set cpuclk src to the CPU pll
}

void pre_ccu_sys(uint32_t clk)
{
    uint32_t k = 2;
    uint32_t m = 2;
    uint32_t val;
    
    if(clk > 768000000)
    {
        m = 1;
    }
    
    /* Deinit AHB_APB_HCLKC_CFG_REG */
    CCU->AHB1_APB1_CFG = (1<<12)|(0<<8)|(0<<6)|(1<<4);
    
    val = 1 << 31;                                  // enable periph0 pll
    val |= (((clk / (24000000 * k / m)) -1) & 0x1f)<< 8;    // set pll factor_n
    val |= ((k - 1) & 0x03) << 4;                   // set pll factor_k
    val |= ((m - 1) & 0x03) << 0;                   // set pll factor_m
    CCU->PLL_PERIPH0_CTRL = val;                    // apply changes
    while(((CCU->PLL_PERIPH0_CTRL>>28)&0x01)!=1);   // wait pll stable
    
    /* Set AHB1 APB1 CLK
     *  AHB1_CLK_SRC = PLL_PERIPH/AHB_PRE_DIV 
     *  APB1_CLK = AHB1_CLK/2
     *  AHB1_CLK = AHB1_CLK_SRC/1
     *  AHB1_RATIO_CLK = AHB1_CLK_SRC/1
     */
    CCU->AHB1_APB1_CFG = (3<<12)|(1<<8)|(2<<6)|(0<<4)|(0<<0);
    
    /* Set AHB2 CLK
     *  AHB2_CLK_SRC = AHB1_CLK
     */
    CCU->AHB2_CFG = 0<<0;
    
    val = 1 << 12;                                  // enable mbus clk
    val |= 3 << 12;                                 // set mbus clk_src to the periph0
    val |= 1 << 0;                                  // set mbus clk = (periph0)/2
    CCU->MBUS_CLK = val;
    
}
    
// void pre_ccu_mbus(uint32_t clk)
// {
//     uint32_t k = 1;
//     uint32_t m = 1;
//     uint32_t val;
//     
//     if(clk > 768000000)
//     {
//         m = 1;
//     }
//     
//     val = 1 << 31                                   // enable periph0 pll
//     val |= (((clk / (24000000 * k / m)) -1) & 0x1f)<< 8;    // set pll factor_n
//     val |= ((k - 1) & 0x03) << 4                    // set pll factor_k
//     val |= ((m - 1) & 0x03) << 0                    // set pll factor_m
//     CCU->PLL_DDR_CTRL = val;                        // apply changes
//     while(((CCU->PLL_DDR_CTRL>>28)&0x01)!=1);       // wait pll stable
//     
//     val = 3 << 12;                                  // enable mbus clk
//     val |= 3 << 12;                                 // set mbus clk_src to the pll_ddr
//     val |= 1 << 0;                                  // set mbus clk = (pll_ddr)/2
//     CCU->MBUS_CLK = 0x81000001;
// }
    
    
    
