#include <string.h>

size_t strlen(const char *s)
{
    const char *len;
    for(len = s; *len != '\0'; len++);
    return len - s;
}
