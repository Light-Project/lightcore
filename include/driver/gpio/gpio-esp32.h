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

#ifndef _CHIP_GPIO_ESP32_H_
#define _CHIP_GPIO_ESP32_H_

#include <types.h>

enum GPIO_ESP32_Registers
{
    GPIO_ESP32_BT_SELECT_REG             = 0x0000,
    GPIO_ESP32_OUT_REG                   = 0x0004,
    GPIO_ESP32_OUT_W1TS_REG              = 0x0008,
    GPIO_ESP32_OUT_W1TC_REG              = 0x000c,
    GPIO_ESP32_OUT1_REG                  = 0x0010,
    GPIO_ESP32_OUT1_W1TS_REG             = 0x0014,
    GPIO_ESP32_OUT1_W1TC_REG             = 0x0018,
    GPIO_ESP32_SDIO_SELECT_REG           = 0x001c,
    GPIO_ESP32_ENABLE_REG                = 0x0020,
    GPIO_ESP32_ENABLE_W1TS_REG           = 0x0024,
    GPIO_ESP32_ENABLE_W1TC_REG           = 0x0028,
    GPIO_ESP32_ENABLE1_REG               = 0x002c,
    GPIO_ESP32_ENABLE1_W1TS_REG          = 0x0030,
    GPIO_ESP32_ENABLE1_W1TC_REG          = 0x0034,
    GPIO_ESP32_STRAP_REG                 = 0x0038,
    GPIO_ESP32_IN_REG                    = 0x003c,
    GPIO_ESP32_IN1_REG                   = 0x0040,
    GPIO_ESP32_STATUS_REG                = 0x0044,
    GPIO_ESP32_STATUS_W1TS_REG           = 0x0048,
    GPIO_ESP32_STATUS_W1TC_REG           = 0x004c,
    GPIO_ESP32_STATUS1_REG               = 0x0050,
    GPIO_ESP32_STATUS1_W1TS_REG          = 0x0054,
    GPIO_ESP32_STATUS1_W1TC_REG          = 0x0058,
    GPIO_ESP32_ACPU_INT_REG              = 0x0060,
    GPIO_ESP32_ACPU_NMI_INT_REG          = 0x0064,
    GPIO_ESP32_PCPU_INT_REG              = 0x0068,
    GPIO_ESP32_PCPU_NMI_INT_REG          = 0x006c,
    GPIO_ESP32_CPUSDIO_INT_REG           = 0x0070,
    GPIO_ESP32_ACPU_INT1_REG             = 0x0074,
    GPIO_ESP32_ACPU_NMI_INT1_REG         = 0x0078,
    GPIO_ESP32_PCPU_INT1_REG             = 0x007c,
    GPIO_ESP32_PCPU_NMI_INT1_REG         = 0x0080,
    GPIO_ESP32_CPUSDIO_INT1_REG          = 0x0084,
    GPIO_ESP32_PINX_REG                  = 0x0088,
    GPIO_ESP32_cali_conf_REG             = 0x0128,
    GPIO_ESP32_cali_data_REG             = 0x012c,
    GPIO_ESP32_FUNCX_IN_SEL_CFG_REG      = 0x0130,
    GPIO_ESP32_FUNCX_OUT_SEL_CFG_REG     = 0x0580,
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

/* conf0 bitflag definitions */
#define UART_ESP_CONF0_DEFAULT       0x0100001c
#define UART_ESP_CONF0_TICK_AHB      (1<<27)         //  inverse uart dtr level
#define UART_ESP_CONF0_INVAL_DTR     (1<<24)         //  inverse uart dtr level
#define UART_ESP_CONF0_INVAL_RTS     (1<<23)         //  inverse uart rts level
#define UART_ESP_CONF0_INVAL_TXD     (1<<22)         //  inverse uart txd level
#define UART_ESP_CONF0_INVAL_DSR     (1<<21)         //  inverse uart dsr level
#define UART_ESP_CONF0_INVAL_CTS     (1<<20)         //  inverse uart cts level
#define UART_ESP_CONF0_INVAL_RXD     (1<<19)         //  inverse uart rxd level
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

#define UART_ESP_CONF1_DEFAULT       0x6060
#define UART_ESP_LOWPULSE_DEFAULT    0x000fffff
#define UART_ESP_HIGHPULSE_DEFAULT   0x000fffff
#define UART_ESP_RXDCNT_DEFAULT      0x00

#endif /* _CHIP_GPIO_ESP32_H_ */
