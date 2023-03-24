/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SIZE_H_
#define _SIZE_H_

#include <const.h>

#define SZ_1Byte        0x00000001
#define SZ_2Byte        0x00000002
#define SZ_4Byte        0x00000004
#define SZ_8Byte        0x00000008
#define SZ_16Byte       0x00000010
#define SZ_32Byte       0x00000020
#define SZ_64Byte       0x00000040
#define SZ_128Byte      0x00000080
#define SZ_256Byte      0x00000100
#define SZ_512Byte      0x00000200

#define SZ_1KiB         0x00000400
#define SZ_2KiB         0x00000800
#define SZ_4KiB         0x00001000
#define SZ_8KiB         0x00002000
#define SZ_16KiB        0x00004000
#define SZ_32KiB        0x00008000
#define SZ_64KiB        0x00010000
#define SZ_128KiB       0x00020000
#define SZ_256KiB       0x00040000
#define SZ_512KiB       0x00080000

#define SZ_1MiB         0x00100000
#define SZ_2MiB         0x00200000
#define SZ_4MiB         0x00400000
#define SZ_8MiB         0x00800000
#define SZ_16MiB        0x01000000
#define SZ_32MiB        0x02000000
#define SZ_64MiB        0x04000000
#define SZ_128MiB       0x08000000
#define SZ_256MiB       0x10000000
#define SZ_512MiB       0x20000000

#define SZ_1GiB         _AC(0x0010000000, ULL)
#define SZ_2GiB         _AC(0x0020000000, ULL)
#define SZ_4GiB         _AC(0x0040000000, ULL)
#define SZ_8GiB         _AC(0x0080000000, ULL)
#define SZ_16GiB        _AC(0x0100000000, ULL)
#define SZ_32GiB        _AC(0x0200000000, ULL)
#define SZ_64GiB        _AC(0x0400000000, ULL)
#define SZ_128GiB       _AC(0x0800000000, ULL)
#define SZ_256GiB       _AC(0x1000000000, ULL)
#define SZ_512GiB       _AC(0x2000000000, ULL)

#define SZ_1TiB         _AC(0x0010000000000, ULL)
#define SZ_2TiB         _AC(0x0020000000000, ULL)
#define SZ_4TiB         _AC(0x0040000000000, ULL)
#define SZ_8TiB         _AC(0x0080000000000, ULL)
#define SZ_16TiB        _AC(0x0100000000000, ULL)
#define SZ_32TiB        _AC(0x0200000000000, ULL)
#define SZ_64TiB        _AC(0x0400000000000, ULL)
#define SZ_128TiB       _AC(0x0800000000000, ULL)
#define SZ_256TiB       _AC(0x1000000000000, ULL)
#define SZ_512TiB       _AC(0x2000000000000, ULL)

#endif /* _SIZE_H_ */
