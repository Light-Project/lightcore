#ifndef _EXPORT_H_
#define _EXPORT_H_

struct kernel_symbot
{
    const char *name;
    void *addr;
};

#define EXPORT_SYMBOL(symbol)                           \
    extern typeof(symbol) symbol;                       \
    static const char __ksym_name_##symbol[] = #symbol; \
    static const struct kernel_symbot __ksymtab_##symbol\
    __attribute__((__used__, section(".ksymtab"))) =    \
    {                                                   \
        .name = __ksym_name_##symbol,                   \
        .addr = (void *)symbol,                         \
    }

#endif
