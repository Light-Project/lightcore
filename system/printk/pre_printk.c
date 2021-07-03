#include <printk.h>
#include <console.h>

struct console *pre_console;

void pre_printk(const char *str, ...)
{
    char buf[256];
    int len;
    va_list ap;

    if(!pre_console)
        return;

    va_start(ap, str);
    len = vsnprintf(buf, sizeof(buf), str, ap);
    va_end(ap);

    pre_console->write(pre_console, buf, len);
}
