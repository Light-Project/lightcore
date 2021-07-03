#include <boot.h> 

uint8_t nor_read_id(uint8_t *buff)
{
    uint8_t ins[4];
    ins[0] = 0x90;
    ins[1] = 0x40;
    ins[2] = 0x5a;
    ins[3] = 0xa5;
    
    spi_cs();
    spi_transmit(ins,0,sizeof(ins));
    spi_transmit(0,buff,3);
    spi_decs();
    return 0;
}

uint8_t nor_read(uint8_t *buff,uint32_t add,uint32_t len)
{
    uint8_t ins[5];
    uint32_t size;
    ins[0] = 0x01;
    ins[1] = 0x02;
    ins[2] = 0x03;
    ins[3] = 0x04;
    ins[4] = 0x05;
//     ins[1] = (add>>16)&0xff;
//     ins[2] = (add>>8)&0xff;
//     ins[3] = (add>>0)&0xff;
//     ins[4] = 0x00;
    
    spi_cs();
    spi_transmit(ins,0,5);
    
//     while(len)
//     {
//         size=len;
//         if(size>64)
//             size=64;
//         spi_tx(zero,size);
//         buff=spi_rx(buff,size);
//     }
    spi_decs();
    return 0;
}
