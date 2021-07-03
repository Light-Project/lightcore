
#include <crc/crc.h>
#include <crc/crc32_table.h>    /* for crc32 */

uint32_t crc32(const uint8_t *s, int len, uint32_t crc)
{
    while (len--)
        crc = crc32_table[(crc ^ *s++) & 0xff] ^ (crc >> 8);
    return crc;
} 
