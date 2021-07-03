#include <stddef.h>

char * rsize(char * buf, double size)
{
    const char * unit[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int count = ARRAY_SIZE(unit);

    while((size > 1024) && (count--))
    {
        size /= 1024;
    }
    sprintk(buf, "%.3f%s", size, unit[count]);
    return buf;
}

