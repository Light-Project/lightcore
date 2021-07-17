#include <delay.h> 

void msleep(unsigned int ms)
{
    ms *= 0x100000;
    while(ms--);
}
