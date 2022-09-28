#ifndef _GSIZE_H_
#define _GSIZE_H_

#include <types.h>
#include <size.h>

#define GSIZE_BUFF sizeof("1023.45MiB")
#define GFREQ_BUFF sizeof("1023.45GHz")

extern int gunit(char *buff, uintmax_t value, unsigned int step, const char *unit[], unsigned int units, size_t bsize);
extern int gsize(char *buff, uintmax_t value);
extern int gfreq(char *buff, uintmax_t value);

#endif /* _GSIZE_H_ */
