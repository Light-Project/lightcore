/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BOOT_H_
#define _BOOT_H_

#include <lib.h>
#include "romcall.h"

#define MHZ         1000000UL
#define OSC_FREQ    (26 * MHZ)
#define CPU_FREQ    (OSC_FREQ * 4)
#define APB_FREQ    (OSC_FREQ * 2)
#define TIME_FREQ   (APB_FREQ >> 8)
#define UART_BAUD   115200

#define CCU_BASE    ((void *)0x3ff00014)
#define UART_BASE   ((void *)0x60000000)
#define TIMER_BASE  ((void *)0x60000600)
#define WDT_BASE    ((void *)0x60000900)

void __noreturn kboot_start(void *jump);
void __noreturn halt(void);

extern void timer_init(void);
extern void timer_stop(void);
extern void mdelay(uint32_t ms);
extern uint32_t time_read(void);

/* driver/uart.c */
extern void uart_init(void);
extern void uart_deinit(void);
extern void uart_print(const char *);

#endif  /* _BOOT_H_ */
