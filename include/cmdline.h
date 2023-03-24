#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#include <types.h>
#include <stddef.h>

extern uint64_t memparse(const char *ptr, char **endptr);
extern uint64_t timeparse(const char *ptr, char **endptr);
extern int escape_string(char *buff, size_t size, const char *string);
extern unsigned int levenshtein(const char *str1, const char *str2, size_t len1, size_t len2, gfp_t gfp, unsigned int s, unsigned int w, unsigned int a, unsigned int d);

#endif /* _CMDLINE_H_ */
