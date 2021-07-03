#include <string.h> 

int strcmp(const char *s1, const char *s2)
{
    int cp = 0;
    while((!(cp = *s1++ - *s2++)) && (*s1 != '\0'));
    return cp;
}

