#include <crc/crc.h> 
#include <crc/crc16_table.h>

uint16_t crc16(uint8_t *buff, int len, uint16_t crc)
{
    while(len--)
        crc = (crc>>8)^crc16_table[(crc&0xff)^*buff++];
    return crc;
}
