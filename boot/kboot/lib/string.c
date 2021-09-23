/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <lib.h>

/*
 * String examination
 */
__weak size_t strlen(const char *s)
{
    const char *len;
    for(len = s; *len != '\0'; len++);
    return len - s;
}

__weak size_t strnlen(const char *s, size_t n)
{
    const char *sc;

    for (sc = s; n-- && *sc != '\0'; ++sc);
    return sc - s;
}

__weak int strcmp(const char *s1, const char *s2)
{
    int cp = 0;
    while((!(cp = *s1++ - *s2++)) && (*s1 != '\0'));
    return cp;
}

__weak int strncmp(const char *s1, const char *s2, size_t n)
{
    int __res = 0;

    while (n)
    {
        if ((__res = *s1 - *s2++) != 0 || !*s1++)
            break;
        n--;
    }
    return __res;
}

__weak char *strrchr(const char *s, int c)
{
    const char *p = s + strlen(s);

    do {
    if (*p == (char)c)
        return (char *)p;
    } while (--p >= s);

    return NULL;
}

/*
 * Memory manipulation
 */
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

__weak void *memmove(void * dest, const void * src, size_t n)
{
    char * tmp;
    const char * s;

    if (dest <= src)
    {
        tmp = dest;
        s = src;
        while (n--)
            *tmp++ = *s++;
    }
    else{
        tmp = dest;
        tmp += n;
        s = src;
        s += n;
        while (n--)
            *--tmp = *--s;
    }
    return dest;
}

__weak void *memchr(const void *s, int c, size_t n)
{
    char *str = (char *)s;

    while (n-- != 0)
        if (c == *str++)
            return str - 1;

    return NULL;
}

__weak int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for (su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, n--)
        if ((res = *su1 - *su2) != 0)
            break;

    return res;
}
