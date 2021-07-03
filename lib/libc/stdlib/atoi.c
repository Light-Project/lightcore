#include <mm.h>

static char isspace(int x)
{
    if(x==' '||x=='\t'||x=='\n'||x=='\f'||x=='\b'||x=='\r')
        return 1;
    return 0;
}

static char isdigit(int x)
{
    if(x<='9'&&x>='0')         
        return 1; 
    return 0;
}
int atoi(const char *nptr)
{
    int c;                  /* current char */
    int total;              /* current total */
    int sign;               /* if '-', then negative, otherwise positive */
 
    /* skip whitespace */
    while ( isspace((int)(unsigned char)*nptr) )
        ++nptr;
 
    c = (int)(unsigned char)*nptr++;
    sign = c;           /* save sign indication */
    if (c == '-' || c == '+')
        c = (int)(unsigned char)*nptr++;    /* skip sign */
 
    total = 0;
 
    while (isdigit(c)) {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char)*nptr++;    /* get next char */
    }
 
    if (sign == '-')
        return -total;
    else
        return total;   /* return result, negated if necessary */
}
