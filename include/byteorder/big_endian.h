/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BYTEORDER_BIG_ENDIAN_H_
#define _BYTEORDER_BIG_ENDIAN_H_

#include <lightcore/byteorder/big_endian.h>

#ifndef CONFIG_CPU_LITTLE_ENDIAN
#warning inconsistent configuration, needs CONFIG_CPU_LITTLE_ENDIAN
#endif

#include <byteorder/generic.h>
#endif /* _BYTEORDER_BIG_ENDIAN_H_ */

