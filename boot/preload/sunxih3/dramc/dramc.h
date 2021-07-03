#ifndef _DRAMC_H_
#define _DRAMC_H_

#include <types.h>

typedef struct {
    uint16_t page_size;
    uint8_t bus_full_width;
    uint8_t dual_rank;
    uint8_t row_bits;
    uint8_t bank_bits;
    uint8_t dx_read_delays[4][11];
    uint8_t dx_write_delays[4][11];
    uint8_t ac_delays[31];
    uint8_t res[3];
} dramc_typedef;

extern uint32_t dram_clk;

void dram_set_cr(dramc_typedef * para);

void dram_clk_init(uint32_t clk);

void dram_type(dramc_typedef * para);

#endif
