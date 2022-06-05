/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BYTEORDER_H_
#define _BYTEORDER_H_

#include <bitops.h>
#include <lightcore/asm/byteorder.h>

#define _cpu_to_le_type(type)       cpu_to_le##type
#define cpu_to_le_type(type)        _cpu_to_le_type(type)
#define _cpu_to_be_type(type)       cpu_to_be##type
#define cpu_to_be_type(type)        _cpu_to_be_type(type)
#define _le_type_to_cpu(type)       le##type##_to_cpu
#define le_type_to_cpu(type)        _le_type_to_cpu(type)
#define _be_type_to_cpu(type)       be##type##_to_cpu
#define be_type_to_cpu(type)        _be_type_to_cpu(type)

#define cpu_to_le_short(val)        cpu_to_le_type(BITS_PER_SHORT)(val)
#define cpu_to_le_int(val)          cpu_to_le_type(BITS_PER_INT)(val)
#define cpu_to_le_long(val)         cpu_to_le_type(BITS_PER_LONG)(val)
#define cpu_to_le_long_long(val)    cpu_to_le_type(BITS_PER_LONG_LONG)(val)

#define cpu_to_be_short(val)        cpu_to_be_type(BITS_PER_SHORT)(val)
#define cpu_to_be_int(val)          cpu_to_be_type(BITS_PER_INT)(val)
#define cpu_to_be_long(val)         cpu_to_be_type(BITS_PER_LONG)(val)
#define cpu_to_be_long_long(val)    cpu_to_be_type(BITS_PER_LONG_LONG)(val)

#define le_short_to_cpu(val)        le_type_to_cpu(BITS_PER_SHORT)(val)
#define le_int_to_cpu(val)          le_type_to_cpu(BITS_PER_INT)(val)
#define le_long_to_cpu(val)         le_type_to_cpu(BITS_PER_LONG)(val)
#define le_long_long_to_cpu(val)    le_type_to_cpu(BITS_PER_LONG_LONG)(val)

#define be_short_to_cpu(val)        be_type_to_cpu(BITS_PER_SHORT)(val)
#define be_int_to_cpu(val)          be_type_to_cpu(BITS_PER_INT)(val)
#define be_long_to_cpu(val)         be_type_to_cpu(BITS_PER_LONG)(val)
#define be_long_long_to_cpu(val)    be_type_to_cpu(BITS_PER_LONG_LONG)(val)

#endif  /* _BYTEORDER_H_ */
