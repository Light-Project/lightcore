/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BOOT_H_
#define _BOOT_H_

#include <mm.h>
#include <lib.h>
#include <asm/pgtable.h>

#define MHZ         1000000UL
#define osc_freq    (27 * MHZ)
#define dto_freq    (1152 * MHZ)
#define dvb_freq    (1152 * MHZ)
#define cpu_freq    (CONFIG_PRELOAD_CPU_CLK * MHZ)
#define axi_freq    (CONFIG_PRELOAD_AXI_CLK * MHZ)
#define ahb_freq    (CONFIG_PRELOAD_AHB_CLK * MHZ)
#define apb_freq    (CONFIG_PRELOAD_APB_CLK * MHZ)
#define dram_freq   (CONFIG_PRELOAD_DRAM_CLK * MHZ)
#define clock_uart  (CONFIG_PRELOAD_UART_CLK)

#define WDT_BASE    ((void *)(SSEG1_BASE + 0x0020b000))
#define SPI_BASE    ((void *)(SSEG1_BASE + 0x00302000))
#define GCTL_BASE   ((void *)(SSEG1_BASE + 0x0030a000))
#define SER_BASE    ((void *)(SSEG1_BASE + 0x00400000))
#define DRAMC_BASE  ((void *)(SSEG1_BASE + 0x00c00000))
#define DRAM_BASE   ((void *)(SSEG1_BASE + 0x10000000))

#define head_addr   0x2000

void kboot_start(void *jump);
void halt(void);

uint32_t ccu_init(void);
uint32_t ccu_cpu(uint32_t dto, uint32_t freq);
uint32_t ccu_axi(uint32_t cpu, uint32_t freq);
uint32_t ccu_ahb(uint32_t cpu, uint32_t freq);
uint32_t ccu_apb(uint32_t dto, uint32_t freq);
uint32_t ccu_dram(uint32_t freq);

void dramc_init(void);

void uart_print(const char *str);
void uart_sync(void);
void uart_init(uint32_t apb, uint32_t freq);

void spi_sel(bool sel);
void spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len);
void spi_init(void);
void spi_deinit(void);

bool norflash_id(void);
void norflash_read(uint8_t *buff, size_t addr, uint32_t len);

void __noreturn wdt_reboot(void);

#endif
