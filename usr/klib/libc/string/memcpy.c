#include <string.h>

void *memcpy(void *dest, const void* src, size_t len)
{   
    char *ndest = (char *)dest;
    char *nsrc = (char *)src;

    if(ndest==NULL || nsrc==NULL || len==0)
        return NULL;
    
    while(len--)
        *ndest++ = *nsrc++;
    
    return dest;
}
