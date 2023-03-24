/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PARAM_H_
#define _PARAM_H_

#include <types.h>

typedef state (*param_entry_t)(char *param, char *val, void *pdata);

extern char *param_next_arg(char *args, char **param, char **val);
extern state param_parser(char *cmdline, param_entry_t entry, void *pdata);

#endif /* _PARAM_H_ */
