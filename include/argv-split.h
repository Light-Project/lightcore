/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARGV_SPLIT_H_
#define _ARGV_SPLIT_H_

#include <types.h>

extern unsigned int argv_count(const char *string);
extern char **argv_split(const char *string, unsigned int *argcp, gfp_t flags);
extern int argv_merge(int argc, char *argv[], char *buff, int size);

#endif /* _ARGV_SPLIT_H_ */
