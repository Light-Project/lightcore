#ifndef _GSIZE_H_
#define _GSIZE_H_

#include <types.h>
#include <size.h>

#define GSIZE_BUFF sizeof("1023.45Byte")
extern int gsize(char *buff, size_t size);

#endif /* _GSIZE_H_ */
