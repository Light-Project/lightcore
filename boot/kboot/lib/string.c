#include <lib.h>

__weak void *memcpy(void *dest, const void* src, size_t len)
{   
    uint8_t *ndest = (uint8_t *)dest;
    uint8_t *nsrc = (uint8_t *)src;

    if(ndest==NULL || nsrc==NULL || len==0)
        return NULL;
    
    while(len--)
        *ndest++ = *nsrc++;
    
    return dest;
}

__weak void *memset(void * s, int c, size_t n)
{
    char * xs = s;

    while (n--)
        *xs++ = c;

    return s;
}
__weak int strcmp(const char *s1, const char *s2)
{
    int cp = 0;
    while((!(cp = *s1++ - *s2++)) && (*s1 != '\0'));
    return cp;
}

__weak size_t strlen(const char *s)
{
    const char *len;
    for(len = s; *len != '\0'; len++);
    return len - s;
}
