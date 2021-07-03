#include <crc/crc.h> 
#include <crc/crc4_table.h>

uint8_t crc4(uint8_t *buff, uint8_t crc, uint32_t len)
{
    len *= 2;
    while(len--)
        crc = crc4_table[(crc&0xf)^*buff++];
    return crc;
}
