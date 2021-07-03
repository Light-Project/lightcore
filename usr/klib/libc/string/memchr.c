#include <string.h>

void *memchr(const void *s, int c, size_t n)
{
    char *str = (char *)s;
    while (n-- != 0)
    {
        if (c == *str++)
        {
            return str - 1;
        }
    }

    return NULL;
}
