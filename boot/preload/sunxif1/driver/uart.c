#include "boot.h"
#include <chip/sunxif1.h> 

void uart_init()
{
    
    uint32_t clk = (clock_apb/clock_uart)/16;
    
    /* Set uart0 gpio : PE0->RX PE1->TX */
    CHIP_PE->CFG0 &= ~((0x0F<<4)|(0x0F<<0)); 
    CHIP_PE->CFG0 |= (0x05<<4)|(0x05<<0);
    
    /* Open UART0 clock */
    CHIP_CCU->BUS_CLK_GATING_REG2 |= 1<<20;
    
    /* Reset UART0 module */
    CHIP_CCU->BUS_SOFT_RST_REG2 |= 1<<20;
    
    /* Confug UART1 mode : 115200 8N1 */
    CHIP_UART0->DLH_IER = 0x00;                 // Clean DLH_IER
    CHIP_UART0->IIR_FCR = 0xF7;                 // Set FEFLAG to enable fifo  Set IID to disable modem
    CHIP_UART0->MCR     = 0x00;                 // Clean MCR
    
    CHIP_UART0->LCR  |= 1<<7;                   // SET DLAB to the UART ferq mode
    CHIP_UART0->RBR_THR_DLL = (clk & 0xff);     // DLL(low reg): 360M/(16*13)=115384 near to 115200
    CHIP_UART0->DLH_IER = (clk >> 8) & 0xff;    // DLH(high reg)
    
    CHIP_UART0->LCR  &= ~(1<<7);                // Reset DLAB to the communication mode
    CHIP_UART0->LCR  |= 0x03<<0;                // Set DLS len 8bits
    
}

void uart_putc(uint8_t str)
{
    while(((CHIP_UART0->USR)&(0x01<<1))==0);    // Wait transmit fifo free
        CHIP_UART0->RBR_THR_DLL = str;          // Write data to the transmit fifo
}

void stdout(uint8_t *str)
{
    while(*str)
    {
        uart_putc(*str++);
    }
}
