#include <string.h>
#include <stdlib.h>

char * strdup(const char * s)
{
    char * p;

    if(!s)
        return NULL;

    p = malloc(strlen(s) + 1);
    if(p)
        return(strcpy(p, s));

    return NULL;
}
