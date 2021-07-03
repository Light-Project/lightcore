#ifndef _RISCV64VIRT_H_
#define _RISCV64VIRT_H_

#include <types.h>

/*
/*
* UART Register structure
*/

typedef struct{
    union{
    uint32_t RBR;           // 0x00 rx-buffer
    uint32_t THR;           // 0x00 tx-buffer
    uint32_t DLR_LSB;       // 0x00 feq-low
    };
    union{
    uint32_t DLR_MSB;       // 0x04 feq-high
    uint32_t IER;           // 0x04 int-ctl
    };
    union{
    uint32_t IIR;           // 0x08 int-ide
    uint32_t FCR;           // 0x08 fifo-ctl
    };
    uint32_t LCR;           // 0x0c line-ctl
    uint32_t MCR;           // 0x10 modem-ctl
    uint32_t LSR;           // 0x14 line-status
    uint32_t MSR;           // 0x18 modem-status
    uint32_t SCH;           // 0x1c cache
    uint8_t  RESERVED0[92]; // 0x20-0x7b res
    uint32_t USR;           // 0x7c uart status
    uint32_t TFL;           // 0x80 tx-fifo level
    uint32_t RFL;           // 0x84 rx-fifo level
    uint8_t  RESERVED1[28]; // 0x88 res
    uint32_t HALT;          // 0xa4 dma handshake
} UART_TypeDef;

#define UART0_BASE      ((uint32_t)0x01C25000)

#define CHIP_UART0      ((UART_TypeDef *)UART0_BASE)

#endif 
