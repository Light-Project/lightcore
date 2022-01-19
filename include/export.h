/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _EXPORT_H_
#define _EXPORT_H_

struct kernel_symbol {
    const char *name;
    void *addr;
};

#define __EXPORT_SYMBOL(symbol, sec)                \
    extern typeof(symbol) symbol; static const      \
    struct kernel_symbol __ksymtab_##symbol         \
    __used __section(".ksymtab" #sec) = {           \
        .name = #symbol,                            \
        .addr = (void *)&symbol,                    \
    }

#define EXPORT_SYMBOL(sym)      __EXPORT_SYMBOL(sym, )
#define EXPORT_SYMBOL_GPL(sym)  __EXPORT_SYMBOL(sym, _gpl)

#endif  /* _EXPORT_H_ */
