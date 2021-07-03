#include <boot.h>
#include "dramc.h"
#include <chip/sunxi/dramc.h>

static int dram_test(uint32_t offset)
{
    *((uint32_t *)DRAM_BASE + offset) = 0xaa5500ff;
    __asm__ __volatile__ ("dsb" : : : "memory");
    return (0xaa5500ff == *((uint32_t *)DRAM_BASE + offset));
}

void dram_type(dramc_typedef * para)
{
    para->page_size = 512;  // initial value of memory page size
    para->row_bits = 16;    // initial value of memory chip row
    para->bank_bits = 2;    // initial value of memory chip bank
    dram_set_cr(para);
    for(para->row_bits = 11; para->row_bits < 16; para->row_bits++)
    {
        if(dram_test((1 << (para->row_bits + para->bank_bits)) * para->page_size))
            break;
    }

    para->bank_bits = 3;
    dram_set_cr(para);
    for(para->bank_bits = 2; para->bank_bits < 3; para->bank_bits++)
    {
        if(dram_test((1 << para->bank_bits) * para->page_size))
            break;
    }

    para->page_size = 8192;
    dram_set_cr(para);
    for(para->page_size = 512; para->page_size < 8192; para->page_size *= 2)
    {
        if(dram_test(para->page_size))
            break;
    }
} 
