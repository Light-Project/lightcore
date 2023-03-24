/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _NUMA_H_
#define _NUMA_H_

#include <types.h>

#ifdef CONFIG_NUMA_SHIFT
# define NUMA_SHIFT CONFIG_NUMA_SHIFT
#else
# define NUMA_SHIFT 0
#endif

#define NUMA_NONE   (-1)
#define NUMA_MAX    (1 << NUMA_SHIFT)

#endif /* _NUMA_H_ */
