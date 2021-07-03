#include <types.h>
#include <stddef.h>
#include <hash.h>

uint32_t strhash(const char *str)
{
    uint32_t hash = 5320;
    if(str == NULL)
        return 0;
    while(*str != '\0')
        hash = (hash << 2) + hash + (*str++);
    return hash;
}
