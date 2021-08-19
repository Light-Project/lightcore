/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BOOT_H_
#define _BOOT_H_

#include <types.h>
#include <state.h>
#include <stddef.h>
#include <lib.h>

#define clock_cpu   600000000
#define clock_sys   360000000
#define clock_dram  156000000

#define clock_ahb   clock_sys
#define clock_apb   (clock_ahb / 2)

#define clock_mmc   20000000
#define clock_spi   20000000
#define clock_uart  115200

#define DRAMC_BASE  ((void *)0x01c01000)
#define SPI_BASE    ((void *)0x01c05000)
#define MMC_BASE    ((void *)0x01c0f000)
#define CCU_BASE    ((void *)0x01c20000)
#define PIO_BASE    ((void *)0x01c20800)
#define SER_BASE    ((void *)0x01c25000)
#define DRAM_BASE   ((void *)0x80000000)

#define PIO_A       (PIO_BASE + (0 * 0x24))
#define PIO_B       (PIO_BASE + (1 * 0x24))
#define PIO_C       (PIO_BASE + (2 * 0x24))
#define PIO_D       (PIO_BASE + (3 * 0x24))
#define PIO_E       (PIO_BASE + (4 * 0x24))
#define PIO_F       (PIO_BASE + (5 * 0x24))

void kboot_start(void *jump);

void ccu_cpu(uint32_t freq);
void ccu_sys(uint32_t freq);
void dram_init(uint32_t freq);

void uart_init(uint32_t freq);
void uart_print(const char *str);

void spi_sel(bool sel);
void spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len);
void spi_init(uint32_t freq);

bool norflash_id(void);
void norflash_read(uint8_t *buff, size_t addr, uint32_t len);

void mmc_init(uint32_t freq);

#endif
