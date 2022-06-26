/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _POISON_H_
#define _POISON_H_

#include <const.h>

#ifdef CONFIG_POISON_ILLEGAL_ADDRESS
# define POISON_OFFSET  _AC(CONFIG_POISON_ILLEGAL_ADDRESS, UL)
#else
# define POISON_OFFSET  0
#endif

#define POISON_LIST1    ((void *) POISON_OFFSET + 0x100)
#define POISON_LIST2    ((void *) POISON_OFFSET + 0x110)
#define POISON_HLIST1   ((void *) POISON_OFFSET + 0x120)
#define POISON_HLIST2   ((void *) POISON_OFFSET + 0x130)
#define POISON_SLIST    ((void *) POISON_OFFSET + 0x140)

#define POISON_RBNODE1  ((void *) POISON_OFFSET + 0x400)
#define POISON_RBNODE2  ((void *) POISON_OFFSET + 0x410)
#define POISON_RBNODE3  ((void *) POISON_OFFSET + 0x420)

#define POISON_HPNODE1  ((void *) POISON_OFFSET + 0x500)
#define POISON_HPNODE2  ((void *) POISON_OFFSET + 0x510)
#define POISON_HPNODE3  ((void *) POISON_OFFSET + 0x520)

#endif  /* _POISON_H_ */
