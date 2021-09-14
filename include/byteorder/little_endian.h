/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BYTEORDER_LITTLE_ENDIAN_H_
#define _BYTEORDER_LITTLE_ENDIAN_H_

#include <lightcore/byteorder/little_endian.h>

#ifdef CONFIG_CPU_BIG_ENDIAN
#warning inconsistent configuration, CONFIG_CPU_BIG_ENDIAN is set
#endif

#include <byteorder/generic.h>
#endif /* _BYTEORDER_LITTLE_ENDIAN_H_ */
