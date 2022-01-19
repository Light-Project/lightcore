/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_DEVID_H_
#define _LIGHTCORE_DEVID_H_

#define MINOR_BITS  20
#define MINOR_MASK  ((1U << MINOR_BITS) - 1)

#define MINOR(dev) (dev & MINOR_MASK)
#define MAJOR(dev) (dev >> MINOR_BITS)
#define DEVID(ma, mi) ((ma << MINOR_BITS) | (mi))

#endif  /* _LIGHTCORE_DEVID_H_ */
