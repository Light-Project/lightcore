#include <types.h>
#include <stdarg.h>
#include <stddef.h>
#include <printk.h>
#include <console.h>
#include <init/initcall.h>

static void print_out(char *str, int len)
{      
#ifdef CONFIG_PRE_PRINTK
    if(pre_console)
        pre_console->write(pre_console, str, len);
#endif
}

static void vprintk(char *str, int len)
{
    
    print_out(str, len);
}

int printk(char *str,...)
{
    va_list para;
    char buf[128];
    int len;
    
    va_start(para,str);
    len = vsnprintf(buf, sizeof(buf), str, para);
    va_end(para);

    vprintk(buf, len);

    return len;   
}

