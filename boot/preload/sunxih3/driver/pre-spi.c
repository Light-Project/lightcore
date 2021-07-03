#include <chip/sunxif1.h>
#include <boot.h>

void pre_spi_init()
{
    /* Set SPI0 gpio : PC0->CLK PC1->CS PC2->MISO PC3->MOSI */
    CHIP_PC->CFG0 &= ~((0x0F<<12)|(0x0F<<8)|(0x0F<<4)|(0x0F<<0)); 
    CHIP_PC->CFG0 |= (0x02<<12)|(0x02<<8)|(0x02<<4)|(0x02<<0);
    
    /* Pass SPI0 clock */
    CHIP_CCU->BUS_CLK_GATING_REG0 |= 1<<20;
    
    /* Reset SPI0 module */
    CHIP_CCU->BUS_SOFT_RST_REG0 |= 1<<20;
    
    /* Set Spi0 frequency */
    CHIP_SPI0->CCR = (1<<12)|(((((AHB_CLK/CONFIG_BOOTLOADER_SPI_CLK)/2)-1)<<0)&0xff);
    
    /* module reset */
    CHIP_SPI0->GCR |= 1<<31;
    
    /* enable spi master */
    CHIP_SPI0->GCR |= (1<<7)|(1<<1)|(1<<0);
    
    while(CHIP_SPI0->GCR & (1 << 31));
    
    /* Set transmission level mode */
    CHIP_SPI0->TCR &= ~((1<<1)|(1<<0));
    CHIP_SPI0->TCR |= (1<<6)|(1<<2);
    
    /* Enable tx fifo and rx fifo*/
    CHIP_SPI0->FCR |= (1<<31)|(1<<15);
    
}

void pre_spi_deinit()
{
    /* Reset SPI0 module */
    CHIP_CCU->BUS_SOFT_RST_REG0 |= 1<<20;
    
    /* Mask SPI0 clock */
    CHIP_CCU->BUS_CLK_GATING_REG0 &= ~(1<<20);
    
    
    /* Free SPI0 gpio */
    CHIP_PE->CFG0 |= (0x0F<<12)|(0x0F<<8)|(0x0F<<4)|(0x0F<<0); 
}

uint32_t pre_spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len)
{
    uint32_t count;
    uint8_t null;
    
    if(len>64)
        len=64;
    
    CHIP_SPI0->MBC = len;
    CHIP_SPI0->MTC = len;
    CHIP_SPI0->BCC = len&0xffffff;
    
    if(txbuf)
        for(count=len; count; count--)
            CHIP_SPI0->TXD = (uint8_t)*txbuf++;
    else
        for(count=len; count; count--)
            CHIP_SPI0->TXD = (uint8_t)0xFF;

    CHIP_SPI0->TCR |= 1<<31;
        
    while((CHIP_SPI0->FSR & 0xff) < len);
    
    if(rxbuf)
        for(count=len; count; count--)
            *rxbuf++ = (uint8_t)CHIP_SPI0->RXD;
    else
        for(count=len; count; count--)
            null = (uint8_t)CHIP_SPI0->RXD;
    
    return len;
}

void pre_spi_cs()
{
    CHIP_SPI0->TCR &= ~(0x3 << 4);
    CHIP_SPI0->TCR &= ~(1<<7);
}

void pre_spi_decs()
{
    CHIP_SPI0->TCR &= ~(0x3 << 4);
    CHIP_SPI0->TCR |= ~1<<7;
}
