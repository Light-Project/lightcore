#include <boot.h>
#include <chip/sunxi/dramc.h>
#include "dramc.h"


void dram_set_cr(dramc_typedef * para)
{
    DRAM_COM->CR = MCTL_CR_BL8 | MCTL_CR_INTERLEAVED |
        MCTL_CR_DDR3 | MCTL_CR_2T |
        (para->bank_bits == 3 ? MCTL_CR_EIGHT_BANKS : MCTL_CR_FOUR_BANKS) |
        MCTL_CR_BUS_FULL_WIDTH(para->bus_full_width) |
        (para->dual_rank ? MCTL_CR_DUAL_RANK : MCTL_CR_SINGLE_RANK) |
        MCTL_CR_PAGE_SIZE(para->page_size) |
        MCTL_CR_ROW_BITS(para->row_bits);
} 
