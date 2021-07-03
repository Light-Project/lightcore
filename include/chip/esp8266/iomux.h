 
#ifndef _CHIP_ESP8266_IOMUX_H_
#define _CHIP_ESP8266_IOMUX_H_

#include <types.h>

typedef struct{
    uint32_t SLEEP_OE:      1;      // Bit0: 
    uint32_t SLEEP_SEL:     1;      // Bit1: 
    uint32_t Res1:          1;      // Bit2: 
    uint32_t SLEEP_PULLUP:  1;      // Bit3: 
    uint32_t FUNL:          2;      // Bit4: 
    uint32_t Res2:          1;      // Bit6: 
    uint32_t PULLUP:        1;      // Bit7: 
    uint32_t FUNH:          1;      // Bit8: 
} __packed ESP8266_IOMUX_Typedef;

#define ESP8266_IOMUX_BASE  ((uint32_t)0x60000800)

#define ESP8266_IOMUX_SPI   ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x00))
#define ESP8266_IOMUX_SPI1_CLK_EQU_SYS_CLK  (1<<9)
#define ESP8266_IOMUX_SPI0_CLK_EQU_SYS_CLK  (1<<8)

#define ESP8266_IOMUX12 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x04))
#define ESP8266_IOMUX12_MTDI                0
#define ESP8266_IOMUX12_I2SI_DATA           1
#define ESP8266_IOMUX12_HSPIQ_MISO          2
#define ESP8266_IOMUX12_GPIO12              3
#define ESP8266_IOMUX12_UART0_DTR           16

#define ESP8266_IOMUX13 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x08))
#define ESP8266_IOMUX13_MTCK                0
#define ESP8266_IOMUX13_I2SI_BCK            1
#define ESP8266_IOMUX13_HSPID_MOSI          2
#define ESP8266_IOMUX13_GPIO13              3
#define ESP8266_IOMUX13_UART0_CTS           16

#define ESP8266_IOMUX14 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x0c))
#define ESP8266_IOMUX14_MTMS                0
#define ESP8266_IOMUX14_I2SI_WS             1
#define ESP8266_IOMUX14_HSPI_CLK            2
#define ESP8266_IOMUX14_GPIO14              3
#define ESP8266_IOMUX14_UART0_DSR           16

#define ESP8266_IOMUX15 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x10))
#define ESP8266_IOMUX15_MTDO                0
#define ESP8266_IOMUX15_I2SO_BCK            1
#define ESP8266_IOMUX15_HSPI_CS0            2
#define ESP8266_IOMUX15_GPIO15              3
#define ESP8266_IOMUX15_U0RTS               16

#define ESP8266_IOMUX3  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x14))
#define ESP8266_IOMUX3_U0RXD                0
#define ESP8266_IOMUX3_I2SO_DATA            1
#define ESP8266_IOMUX3_GPIO3                3
#define ESP8266_IOMUX3_CLK_XTAL_BK          16

#define ESP8266_IOMUX1  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x18))
#define ESP8266_IOMUX1_U0TXD                0
#define ESP8266_IOMUX1_SPICS1               1
#define ESP8266_IOMUX1_GPIO1                3
#define ESP8266_IOMUX1_CLK_RTC_BK           16

#define ESP8266_IOMUX6  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x1c))
#define ESP8266_IOMUX6_SDCLK                0
#define ESP8266_IOMUX6_SPICLK               1
#define ESP8266_IOMUX6_GPIO6                3
#define ESP8266_IOMUX6_UART1_CTS            16

#define ESP8266_IOMUX7  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x20))
#define ESP8266_IOMUX7_SDDATA0              0
#define ESP8266_IOMUX7_SPIQ_MISO            1
#define ESP8266_IOMUX7_GPIO7                3
#define ESP8266_IOMUX7_U1TXD                16

#define ESP8266_IOMUX8  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x24))
#define ESP8266_IOMUX8_SDDATA1              0
#define ESP8266_IOMUX8_SPID_MOSI            1
#define ESP8266_IOMUX8_GPIO8                3
#define ESP8266_IOMUX8_U1RXD                16

#define ESP8266_IOMUX9  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x28))
#define ESP8266_IOMUX9_SDDATA2              0
#define ESP8266_IOMUX9_SPIHD                1
#define ESP8266_IOMUX9_GPIO9                3
#define ESP8266_IOMUX9_HSPIHD               16

#define ESP8266_IOMUX10 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x2c))
#define ESP8266_IOMUX10_SDDATA3             0
#define ESP8266_IOMUX10_SPIWP               1
#define ESP8266_IOMUX10_GPIO10              3
#define ESP8266_IOMUX10_HSPIWP              16

#define ESP8266_IOMUX11 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x30))
#define ESP8266_IOMUX11_SDCMD               0
#define ESP8266_IOMUX11_SPICS0              1
#define ESP8266_IOMUX11_GPIO11              3
#define ESP8266_IOMUX11_U1RTS               16

#define ESP8266_IOMUX0  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x34))
#define ESP8266_IOMUX0_GPIO0               0
#define ESP8266_IOMUX0_SPICS2              1
#define ESP8266_IOMUX0_CLK_OUT             16

#define ESP8266_IOMUX2  ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x38))
#define ESP8266_IOMUX2_GPIO2                0
#define ESP8266_IOMUX2_I2SO_WS              1
#define ESP8266_IOMUX2_U1TXD_BK             2
#define ESP8266_IOMUX2_U0TXD_BK             16

#define ESP8266_IOMUX4 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x3c))
#define ESP8266_IOMUX4_GPIO4                0
#define ESP8266_IOMUX4_CLK_XTAL             1

#define ESP8266_IOMUX5 ((ESP8266_IOMUX_Typedef *)(ESP8266_IOMUX_BASE + 0x40))
#define ESP8266_IOMUX5_GPIO5                0
#define ESP8266_IOMUX5_CLK_RTC              1


#endif