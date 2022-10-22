#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#include <types.h>
#include <stddef.h>

extern uint64_t memparse(const char *ptr, char **endptr);
extern int escape_string(char *buff, size_t size, const char *string);

#endif  /* _CMDLINE_H_ */
