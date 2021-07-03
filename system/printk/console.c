#include <types.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <console.h>
#include <printk.h>
#include <list.h>
#include <init/initcall.h>
#include "private.h"

/* the console list head */
SLIST_HEAD(console_list);

void console_write(const char *str, unsigned int len)
{
    struct console *console;
    
    slist_for_each_entry(console, &console_list, list) {
        if (!(console->enable))
            continue;
        if(!console->write)
            continue;
        console->write(console, str, len);
    }
} 


void console_lock(void)
{

    
}

void console_unlock(void)
{

    
}

void console_register(struct console *ncon)
{
    struct console *bcon = NULL;

    slist_for_each_entry(bcon, &console_list, list) {
        if(bcon == ncon)
            pr_warn("console %s%d already register\n\r", 
                    bcon->name, bcon->index);
    }

    console_lock();

    /* add new console to the list */
    slist_add(&console_list, &ncon->list);

    console_unlock();
}

void console_unregister(struct console *dcon)
{
    console_lock();

    slist_del(&console_list, &dcon->list);

    console_unlock();
}

void __init console_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;
    int ret;
    
    for(fn = _ld_console_initcall_start;
        fn < _ld_console_initcall_end;
        fn++)
    {
        call = initcall_from_entry(fn);
        ret = call();
        if(ret)
            pr_warn("console: init error\n\r");
    }
}
