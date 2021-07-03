#ifndef _CHIP_VIRT_UART_
#define _CHIP_VIRT_UART_

#include <types.h>

typedef struct{
    union{
        uint8_t RBR;    // Receive Buffer Register
        uint8_t THR;    // Transmitter Holding Register
        uint8_t DLL;    // Divisor Latch Register LSB
    };
    union{
        uint8_t DLH;    // Divisor Latch Register MSB
        uint8_t IER;    // Interrupt Enable Register
    };
    union{
        uint8_t IIR;    // Interrupt Identification Register
        uint8_t FCR;    // FIFO Control Register
    };

    uint8_t LCR;        // Line Control Register
    uint8_t MCR;        // MODEM Control Register
    uint8_t LSR;        // Line Status Register
    uint8_t MSR;        // MODEM Status Register
} __packed UART_TypeDef ;

#define VIRT_UART_BASE      ((uint32_t)0x10000000)

#define VIRT_UART      ((UART_TypeDef *)VIRT_UART_BASE)

#endif
