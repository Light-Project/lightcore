/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_UART_ESP_H_
#define _DRIVER_UART_ESP_H_

#include <bits.h>

enum esp_uart_registers {
    ESP8266_UART_FIFO       = 0x00,     // 0x00: read by rx fifo, write by tx fifo
    ESP8266_UART_INT_RAW    = 0x04,     // 0x04: uart interrupt raw state
    ESP8266_UART_INT_ST     = 0x08,     // 0x08: uart interrupt state
    ESP8266_UART_INT_ENA    = 0x0c,     // 0x0c: uart interrupt enable register
    ESP8266_UART_INT_CLR    = 0x10,     // 0x10: uart interrupt clear register
    ESP8266_UART_CLKDIV     = 0x14,     // 0x14: uart clk div register
    ESP8266_UART_AUTOBAUD   = 0x18,     // 0x18: uart baudrate detect register
    ESP8266_UART_STATUS     = 0x1c,     // 0x1c: uart status register
    ESP8266_UART_CONF0      = 0x20,     // 0x20: uart config0(uart0 and uart1)
    ESP8266_UART_CONF1      = 0x24,     // 0x24: uart config1
    ESP8266_UART_LOWPULSE   = 0x28,     // 0x28: used uart baudrate detect
    ESP8266_UART_HIGHPULSE  = 0x2c,     // 0x2c: used uart baudrate detect
    ESP8266_UART_RXDCNT     = 0x30,     // 0x30: used uart baudrate detect
};

/***************************************************************************************************/
/*      Mnemonic                        value               meaning/usage                          */

#define ESP8266_UART_CLKDIV_FRAG        0xf00000
#define ESP8266_UART_CLKDIV_DIV         0xfffff

#define ESP8266_UART_STATUS_TXFIFO_CNT  BIT_SHIFT(16)
#define ESP8266_UART_STATUS_RXFIFO_CNT  BIT_SHIFT(0)

#define ESP8266_UART_CONF0_TICK_AHB     BIT(27)             // Inverse Uart DTR Level
#define ESP8266_UART_CONF0_INVAL_DTR    BIT(24)             // Inverse Uart DTR Level
#define ESP8266_UART_CONF0_INVAL_RTS    BIT(23)             // Inverse Uart RTS Level
#define ESP8266_UART_CONF0_INVAL_TXD    BIT(22)             // Inverse Uart TXD Level
#define ESP8266_UART_CONF0_INVAL_DSR    BIT(21)             // Inverse Uart DSR Level
#define ESP8266_UART_CONF0_INVAL_CTS    BIT(20)             // Inverse Uart CTS Level
#define ESP8266_UART_CONF0_INVAL_RXD    BIT(19)             // Inverse Uart RXD Level
#define ESP8266_UART_CONF0_TXFIFO_RST   BIT(18)             // Reset Uart tx fifo
#define ESP8266_UART_CONF0_RXFIFO_RST   BIT(17)             // Reset Uart rx fifo
#define ESP8266_UART_CONF0_TXFLOW_EN    BIT(15)             // Enable Uart tx hardware flow control
#define ESP8266_UART_CONF0_LOOP         BIT(14)             // Enable Uart loopback test mode
#define ESP8266_UART_CONF0_DTR          BIT(7)              // Sw DTR
#define ESP8266_UART_CONF0_RTS          BIT(6)              // Sw RTS
#define ESP8266_UART_CONF0_STB_2        BIT_SHIFT(4, 3)     // Set Stop Bit: 2bits
#define ESP8266_UART_CONF0_STB_1_2      BIT_SHIFT(4, 2)     // Set Stop Bit: 1.5bits
#define ESP8266_UART_CONF0_STB_1        BIT_SHIFT(4, 1)     // Set Stop Bit: 1bits
#define ESP8266_UART_CONF0_WSL_8        BIT_SHIFT(2, 3)     // Set Bit Num: 3:8bits
#define ESP8266_UART_CONF0_WSL_7        BIT_SHIFT(2, 2)     // Set Bit Num: 2:7bits
#define ESP8266_UART_CONF0_WSL_6        BIT_SHIFT(2, 1)     // Set Bit Num: 1:6bits
#define ESP8266_UART_CONF0_WSL_5        BIT_SHIFT(2, 0)     // Set Bit Num: 0:5bits
#define ESP8266_UART_CONF0_PARITY_EN    BIT(1)              // Uart Parity Check
#define ESP8266_UART_CONF0_PARITY_ODD   BIT_SHIFT(0, 1)     // Set Parity Check: Odd
#define ESP8266_UART_CONF0_PARITY_EVEN  BIT_SHIFT(0, 0)     // Set Parity Check: Even

#define ESP8266_UART_INT_ENA_DEFAULT    0x00000000
#define ESP8266_UART_CLKDIV_DEFAULT     0x00000000
#define ESP8266_UART_AUTOBAUD_DEFAULT   0x00001000
#define ESP8266_UART_CONF0_DEFAULT      0x0100001c
#define ESP8266_UART_CONF1_DEFAULT      0x00006060
#define ESP8266_UART_LOWPULSE_DEFAULT   0x000fffff
#define ESP8266_UART_HIGHPULSE_DEFAULT  0x000fffff
#define ESP8266_UART_RXDCNT_DEFAULT     0x00000000

#endif  /* _DRIVER_UART_ESP_H_ */
