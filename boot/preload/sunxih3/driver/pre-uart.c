#include "boot.h"
#include <chip/sunxih3.h> 

void pre_uart_init(void)
{
    /* Set uart0 gpio : PA4 PA5 */
    PA->CFG0 &= ~((0x0F<<20)|(0x0F<<16)); 
    PA->CFG0 |= (0x02<<20)|(0x02<<16);
    
    /* Open UART0 clock */
    CCU->BUS_CLK_GATING_REG3 |= 1<<16;
    
    /* Reset UART0 module */
    CCU->BUS_SOFT_RST_REG4 |= 1<<16;
    
    /* Confug UART0 mode : 115200 8N1 */
    UART0->DLH_IER = 0x00;  // Clean DLH_IER
    UART0->IIR_FCR = 0xF7;  // Set FEFLAG to enable fifo  Set IID to disable modem
    UART0->MCR     = 0x00;  // Clean MCR
    
    UART0->LCR  |= 1<<7;                // SET DLAB to the UART ferq mode
    UART0->RBR_THR_DLL = (0xd & 0xff);  // DLL(low reg): 24M/(16*13)=115384 near to 115200
    UART0->DLH_IER = (0xd >> 8) & 0xff; // DLH(high reg): 0
    
    UART0->LCR  &= ~(1<<7);             // Reset DLAB to the communication mode
    UART0->LCR  |= 0x03<<0;             // Set DLS len 8bits
}

void pre_uart_putc(uint8_t str)
{
    while(((UART0->USR)&(0x01<<1))==0);  // Wait transmit fifo free
    UART0->RBR_THR_DLL = str;       // Write data to the transmit fifo
}

void pre_uart_printf(uint8_t *str)
{
    while(*str)
    {
        pre_uart_putc(*str);
        str++;
    }
}
