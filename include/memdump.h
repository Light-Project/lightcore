#ifndef _MEMDUMP_H_
#define _MEMDUMP_H_

#include <fmtdump.h>

extern int memdump_print(fmtdump_print_t func, void *pdata, const char *prefix, enum fmtdump_prefix_mode mode,
                         const void *data, size_t size, int rowsize, int groupsize, int base, bool ascii);
extern int memdump_printk(const char *prefix, enum fmtdump_prefix_mode mode, const void *data, size_t size,
                          int rowsize, int groupsize, int base, bool ascii);

#endif  /* _MEMDUMP_H */
