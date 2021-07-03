#ifndef _BOOT_H_
#define _BOOT_H_

#include <types.h>

#define clock_cpu   800000000
#define clock_dram  240000000
#define clock_sys   480000000
#define clock_ahb   clock_sys
#define clock_apb   clock_ahb/2
#define clock_spi   10000000
#define clock_uart  115200

char *itoa(int val, char *buf, char type);

void sys_dram_init(uint32_t clk);

uint8_t nor_read_id(uint8_t *buff);

void spi_init();
void spi_deinit();
uint32_t spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len);
void spi_cs();
void spi_decs();

void uart_init(void);
void uart_putc(uint8_t str);
void uart_printf(uint8_t *str);

void ccu_cpu(uint32_t clk);
void ccu_sys(uint32_t clk);

void mmu_enable();
void icache_enable();
void dcache_enable();
void domain_write(uint32_t entry);
void section_link(uint32_t *ttb_table, uint32_t va, uint32_t pa, uint32_t size, uint8_t type);
void ttb_set(uint32_t *addr);

#endif
