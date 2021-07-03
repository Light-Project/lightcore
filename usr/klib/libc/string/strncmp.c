#include <string.h>

int strncmp(const char * s1, const char * s2, size_t n)
{
    int res = 0;

    while (n)
    {
        if ((res = *s1 - *s2++) != 0 || !*s1++)
            break;
        n--;
    }
    return res;
}
