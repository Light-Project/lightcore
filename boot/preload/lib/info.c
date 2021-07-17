#include <boot.h>
#include <stdarg.h>
#include <stddef.h>

void (*stdout)(const char *) = NULL;
void __weak halt(void){while(1);}

int printf(const char *str, ...)
{
    char strbuf[128];
    va_list para;
    int len;
    
    if(!stdout)
        return 0;
    
    va_start(para,str);
    len = vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);
    
    return len;
}

int pr_boot(const char *str, ...)
{
    char strbuf[128];
    va_list para;
    int len;
    
    if(!stdout)
        return 0;
    
    stdout("[load]: ");    
    va_start(para,str);
    len = vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);
    
    return len;
}

void panic(const char *str, ...)
{
    char strbuf[128];
    va_list para;
    
    if(!stdout)
        return;
    
    stdout("[panic]: ");    
    va_start(para,str);
    vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);
    
    halt();
    while(1);
} 

void pr_init(void (*fun)(const char *))
{
    stdout = fun;
    stdout("-------------------\n\r");
    stdout("Lightcore preload!\n\r");
    stdout("-------------------\n\r");
}
