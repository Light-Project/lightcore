/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DYNAMIC_CALL_H_
#define _DYNAMIC_CALL_H_

#include <types.h>

typedef long (*dynamic_call_t)(void);
extern long dynamic_call(dynamic_call_t fn, unsigned int num, void *args[]);

#endif /* _DYNAMIC_CALL_H_ */
