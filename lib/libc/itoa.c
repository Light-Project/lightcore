/* SPDX-License-Identifier: GPL-2.0-or-later */

char *itoa(int val, char *buf, char type)
{
    const char str[] = "0123456789abcdef";
    unsigned char sub, hex, tmp[32], count=0;
    
    if (val<0) {
        sub = 1;
        val=-val;
    }
    
    if(type == 0x10)
        hex = 1;
    
    if (val==0) {
        tmp[0] = '0';
        count++;
    } else while(val!=0) {
        tmp[count] = str[val%type];
        val /= type;
        count++;
    }
    
    if (sub == 1 && hex != 1)
        *buf++ = '-';
    
    if (hex == 1) {
        *buf++ = '0';
        *buf++ = 'x';
    }

    while (count--)
        *buf++ = tmp[count];
    
    *buf++ = '\0';
    
    return buf;
}
