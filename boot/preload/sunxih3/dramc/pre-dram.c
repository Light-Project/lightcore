#include <boot.h>
#include <chip/sunxi/dramc.h>
#include "dramc.h"

uint32_t dram_clk = 0;

void pre_dram_init(uint32_t clk)
{
    dram_clk = clk;
    dramc_typedef para = 
    {
        .dual_rank = 1,
        .bus_full_width = 1,
        .row_bits = 15,
        .bank_bits = 3,
        .page_size = 4096,
        .dx_read_delays = {
            { 18, 18, 18, 18, 18, 18, 18, 18, 18, 0, 0 },
            { 14, 14, 14, 14, 14, 14, 14, 14, 14, 0, 0 },
            { 18, 18, 18, 18, 18, 18, 18, 18, 18, 0, 0 },
            { 14, 14, 14, 14, 14, 14, 14, 14, 14, 0, 0 }
        },
        .dx_write_delays = {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0,  6,  6 }
        },
        .ac_delays = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0
        },
    };
    
    uint32_t* dram_size = (void *)0x00000020;       // read save dram size
    
    if(*dram_size!=0)
        return;
    
    dram_clk_init(clk);                             // init dram clock
    
    // if(dram_channel_init(&para))
	// {
	// 	dram_size[0] = 0;
	// 	return;
	// }

    if(para.dual_rank)
        DRAM_CTL0->ODTMAP = 0x00000303;             // Set ddr rank
    else
        DRAM_CTL0->ODTMAP = 0x00000201;             // Set ddr rank

	DRAM_CTL0->ODTCFG = 0x0c000400;

    DRAM_COM->CCCR |= 1 << 31;

    dram_type(&para);
	dram_set_cr(&para);

    dram_size[0] = (1UL << (para.row_bits + para.bank_bits)) 
                        * para.page_size * (para.dual_rank ? 2 : 1);
}
