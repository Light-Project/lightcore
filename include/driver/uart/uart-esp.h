/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/esp8266/uart.h
 *  ESP8266 uart register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-20      first version 
 * 
 */


#ifndef _CHIP_UART_ESP_
#define _CHIP_UART_ESP_

#include <types.h>

enum UART_ESP_Registers
{
    UART_ESP_FIFO        = 0x00,    // 0x00: read by rx fifo, write by tx fifo
    UART_ESP_INT_RAW     = 0x04,    // 0x04: uart interrupt raw state
    UART_ESP_INT_ST      = 0x08,    // 0x08: uart interrupt state
    UART_ESP_INT_ENA     = 0x0c,    // 0x0c: uart interrupt enable register
    UART_ESP_INT_CLR     = 0x10,    // 0x10: uart interrupt clear register
    UART_ESP_CLKDIV      = 0x14,    // 0x14: uart clk div register
    UART_ESP_AUTOBAUD    = 0x18,    // 0x18: uart baudrate detect register
    UART_ESP_STATUS      = 0x1c,    // 0x1c: uart status register
    UART_ESP_CONF0       = 0x20,    // 0x20: uart config0(uart0 and uart1)
    UART_ESP_CONF1       = 0x24,    // 0x24: uart config1
    UART_ESP_LOWPULSE    = 0x28,    // 0x28: used uart baudrate detect
    UART_ESP_HIGHPULSE   = 0x2c,    // 0x2c: used uart baudrate detect
    UART_ESP_RXDCNT      = 0x30,    // 0x30: used uart baudrate detect
};

/***********************************************************************************************/
/*      Mnemonic                        value           meaning/usage                          */

#define UART_ESP_INT_ENA_DEFAULT     0x00

/* clkdiv bitflag definitions */
#define UART_ESP_CLKDIV_DEFAULT      0x00
#define UART_ESP_CLKDIV_FRAG         0xf00000
#define UART_ESP_CLKDIV_DIV          0xfffff

#define UART_ESP_AUTOBAUD_DEFAULT    0x1000

/* status bitflag definitions */
#define UART_ESP_STATUS_TXFIFO_CNT   (16)
#define UART_ESP_STATUS_RXFIFO_CNT   (0)

/* CONF0 bitflag definitions */
#define UART_ESP_CONF0_DEFAULT       0x0100001c
#define UART_ESP_CONF0_TICK_AHB      (1<<27)         // inverse uart dtr level
#define UART_ESP_CONF0_INVAL_DTR     (1<<24)         // inverse uart dtr level
#define UART_ESP_CONF0_INVAL_RTS     (1<<23)         // inverse uart rts level
#define UART_ESP_CONF0_INVAL_TXD     (1<<22)         // inverse uart txd level
#define UART_ESP_CONF0_INVAL_DSR     (1<<21)         // inverse uart dsr level
#define UART_ESP_CONF0_INVAL_CTS     (1<<20)         // inverse uart cts level
#define UART_ESP_CONF0_INVAL_RXD     (1<<19)         // inverse uart rxd level
#define UART_ESP_CONF0_TXFIFO_RST    (1<<18)         // reset uart tx fifo
#define UART_ESP_CONF0_RXFIFO_RST    (1<<17)         // reset uart rx fifo
#define UART_ESP_CONF0_TXFLOW_EN     (1<<15)         // enable uart tx hardware flow control
#define UART_ESP_CONF0_LOOP          (1<<14)         // enable uart loopback test mode
#define UART_ESP_CONF0_DTR           (1<<7)          // Sw dtr
#define UART_ESP_CONF0_RTS           (1<<6)          // Sw rts
#define UART_ESP_CONF0_STB_2         (3<<4)          // Set stop bit: 3:2bits
#define UART_ESP_CONF0_STB_1_2       (2<<4)          // Set stop bit: 2:1.5bits
#define UART_ESP_CONF0_STB_1         (1<<4)          // Set stop bit: 1:1bits
#define UART_ESP_CONF0_WSL_8         (3<<2)          // Set bit num: 3:8bits
#define UART_ESP_CONF0_WSL_7         (2<<2)          // Set bit num: 2:7bits
#define UART_ESP_CONF0_WSL_6         (1<<2)          // Set bit num: 1:6bits
#define UART_ESP_CONF0_WSL_5         (0<<2)          // Set bit num: 0:5bits
#define UART_ESP_CONF0_PARITY_EN     (1<<1)          // Set this bit to enable uart parity check
#define UART_ESP_CONF0_PARITY_ODD    (1<<0)          // Set parity check: odd
#define UART_ESP_CONF0_PARITY_EVEN   (0<<0)          // Set parity check: even

/* CONF1 bitflag definitions */
#define UART_ESP_CONF1_DEFAULT       0x00006060


#define UART_ESP_LOWPULSE_DEFAULT    0x000fffff
#define UART_ESP_HIGHPULSE_DEFAULT   0x000fffff
#define UART_ESP_RXDCNT_DEFAULT      0x00

#endif
