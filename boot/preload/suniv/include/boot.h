/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BOOT_H_
#define _BOOT_H_

#include <types.h>
#include <state.h>
#include <stddef.h>
#include <bits.h>
#include <lib.h>

#define MHZ         1000000UL
#define osc_freq    (24 * MHZ)
#define periph_freq (600 * MHZ)
#define cpu_freq    (CONFIG_PRELOAD_CPU_CLK * MHZ)
#define sys_freq    (CONFIG_PRELOAD_BUS_CLK * MHZ)
#define dram_freq   (CONFIG_PRELOAD_DRAM_CLK * MHZ)

extern uint32_t     clock_cpu;
extern uint32_t     clock_dram;
extern uint32_t     clock_periph;
extern uint32_t     clock_ahb;
#define clock_apb   (clock_ahb / 2)

#define clock_mmc   (CONFIG_PRELOAD_MMC_CLK * MHZ)
#define clock_spi   (CONFIG_PRELOAD_SPI_CLK * MHZ)
#define clock_uart  CONFIG_PRELOAD_UART_CLK

#define DRAMC_BASE  ((void *)0x01c01000)
#define SPI_BASE    ((void *)0x01c05000)
#define MMC_BASE    ((void *)0x01c0f000)
#define CCU_BASE    ((void *)0x01c20000)
#define INTC_BASE   ((void *)0x01c20400)
#define PIO_BASE    ((void *)0x01c20800)
#define SER_BASE    ((void *)0x01c25000)
#define DRAM_BASE   ((void *)0x80000000)
#define FEL_ENTRY   ((void *)0xffff0020)

#define PIO_A       (PIO_BASE + (0 * 0x24))
#define PIO_B       (PIO_BASE + (1 * 0x24))
#define PIO_C       (PIO_BASE + (2 * 0x24))
#define PIO_D       (PIO_BASE + (3 * 0x24))
#define PIO_E       (PIO_BASE + (4 * 0x24))
#define PIO_F       (PIO_BASE + (5 * 0x24))

#define head_addr   0x8000
#define sdcard_lba  ((0x2000 + head_addr) / 512)

#define MMC_READ    0
#define MMC_WRITE   1

struct mmc_cmd {
    uint8_t  cmdtype;
    uint32_t cmdidx;
    uint32_t cmdarg;
    uint32_t response[4];
};

struct mmc_data {
    void *buff;
    uint32_t blksize;
    uint32_t blkcnt;
    bool rw;
};

static inline void mdelay(uint32_t ms)
{
    uint32_t loop = 4000 * ms;
	asm volatile (
        "1: subs %0, %1, #1\n"
        "bne 1b"
        :"=r" (loop)
        :"0"(loop)
    );
}

void kboot_start(void *jump);

uint32_t ccu_periph(uint32_t freq);
uint32_t ccu_cpu(uint32_t freq);
uint32_t ccu_sys(uint32_t freq);
uint32_t ccu_dram(uint32_t freq);

void dramc_init(uint32_t freq);

void uart_print(const char *str);
void uart_sync(void);
void uart_init(uint32_t freq);

void spi_sel(bool sel);
void spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len);
void spi_init(uint32_t freq);
void spi_deinit(void);

void mmc_send_cmd(struct mmc_cmd *cmd, struct mmc_data *data);
void mmc_init(uint32_t freq);
void mmc_deinit(void);

bool norflash_id(void);
void norflash_read(uint8_t *buff, size_t addr, uint32_t len);

bool sdcard_init(void);
void sdcard_read(uint8_t *buff, uint32_t lba, uint32_t blkcnt);

#endif
