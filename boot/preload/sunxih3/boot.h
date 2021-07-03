#ifndef _BOOT_H_
#define _BOOT_H_

#include <types.h>
#include <generated/autoconf.h>

void pre_uart_init(void);
void pre_uart_putc(uint8_t str);
void pre_uart_printf(uint8_t *str);
void pre_ccu_cpu(uint32_t clk);
void pre_ccu_sys(uint32_t clk);

#endif
