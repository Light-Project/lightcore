#include <types.h>

uint8_t io_in8(uint16_t addr)
{
    uint8_t data = 0;
    asm volatile
    (
        "inb    %1, %0\n\r"
        :"=a"(data):"Nd"(addr):
    );
    return data;
}

uint16_t io_in16(uint16_t addr)
{
    uint16_t data = 0;
    asm volatile
    (
        "inw    %1, %0\n\r"
        :"=a"(data):"Nd"(addr):
    );
    return data;
}


uint32_t io_in32(uint16_t addr)
{
    uint32_t data = 0;
    asm volatile
    (
        "inl    %1, %0\n\r"
        :"=a"(data):"Nd"(addr):
    );
    return data;
}

void io_out8(uint16_t addr, uint8_t data)
{
    asm volatile
    (
        "outb   %1, %0\n\r"
        ::"Nd"(addr),"a"(data):
    );
}

void io_out16(uint16_t addr, uint16_t data)
{
    asm volatile
    (
        "outw   %1, %0\n\r"
        ::"Nd"(addr),"a"(data):
    );
}

void io_out32(uint16_t addr, uint32_t data)
{
    asm volatile
    (
        "outl   %1, %0\n\r"
        ::"Nd"(addr),"a"(data):
    );
}
 
