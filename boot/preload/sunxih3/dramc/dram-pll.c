#include <boot.h>
#include "dramc.h"
#include <chip/sunxih3.h>

static void memctl_pll(uint32_t clk)
{
    uint32_t n = 32;
    uint32_t k = 1;
    uint32_t m = 2;
    uint32_t val=0;
    
    /* ddr pll rate = 24000000 * n * k / m */
    if(clk > 24000000 * k * n / m)
    {
        m = 1;
        if(clk > 24000000 * k * n / m)
        {
            k = 2;
        }
    }
    
    /* Init ddr PLL */
    CCU->PLL_DDR_TUN &= ~(0x7<<24);                         // Clean PLL_LTIME_CTRL
    CCU->PLL_DDR_TUN &= ~(0x7f<<16);                        // Clean PLL_INIT_FREQ_CTRL
    
    CCU->PLL_DDR_TUN |= 0x7<<24;                            // Set PLL lock time
    CCU->PLL_DDR_TUN |= 0x7f<<16;                           // Set PLL Initial Frequency
    
    /* Set ddr PLL */
    val |= 1<<31;                                           // Enable the pll
    val |= 1<<20;                                           // Unlock the pll
    val |= ((((clk / (24000000 * k / m)) - 1) & 0x1f) << 8);// Set factor n
    val |= (((k - 1) & 0x3) << 4);                          // Set factor k
    val |= (((m - 1) & 0x3) << 0);                          // Set factor m
    CCU->PLL_DDR_CTRL = val;
}

void dram_clk_init(uint32_t clk)
{
    /* Prepare for switching clock frequency */
    CCU->MBUS_CLK &= ~(1<<31);                          // Disable Mbus clock gating
    CCU->MBUS_RST &= ~(1<<31);                          // Reset Mbus module
    CCU->BUS_CLK_GATING_REG0 &= ~(1<<14);               // Mask DRAM clock
    CCU->BUS_SOFT_RST_REG0 &= ~(1<<14);                 // Reset DRAM module
    CCU->PLL_DDR_CTRL &= ~(1<<31);                      // Disable DDR pll clock
    CCU->DRAM_CFG &= ~(1<<31);                          // Reset DDR ctl reg    
    
    /* Start to switching clock frequency */
    memctl_pll(clk);
    
    /* Switch successfully, return to initial setting */
    CCU->DRAM_CFG &= ~((0x03<<20)|(0x0f<<0));           // Set dram clk_src to ddr_pll | clkdiv = 1
    CCU->DRAM_CFG |= 1<<16;                             // Update the config
    CCU->BUS_SOFT_RST_REG0 |= 1<<14;                    // Dereset DRAM module
    CCU->BUS_CLK_GATING_REG0 |= 1<<14;                  // Pass DRAM clock
    CCU->MBUS_RST |= 1<<31;                             // Dereset Mbus module
    CCU->MBUS_CLK |= 1<<31;                             // Enable Mbus clock gating to Set the module
    CCU->DRAM_CFG |= 1<<31;                             // Enable dram ctl
} 
