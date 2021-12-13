/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_GFP_H_
#define _MM_GFP_H_

#include <types.h>
#include <bits.h>

#ifndef __ASSEMBLY__

/* Physical address region bitmasks */
#define GFP_NORMAL              0x00
#define GFP_DMA                 BIT(0)
#define GFP_DMA32               BIT(1)
#define GFP_HIGHMEM             BIT(2)
#define GFP_MOVABLE             BIT(3)
#define GFP_REGION_MASK         BIT_RANGE(3, 0)

/* Reclaim modifier bitmasks */
#define GFP_IO                  BIT(7)
#define GFP_FS                  BIT(8)
#define GFP_ZERO                BIT(9)
#define GFP_DIRECT_RECLAIM      BIT(11)
#define GFP_KSWAPD_RECLAIM      BIT(12)
#define GFP_NOFAIL              BIT(16)
#define GFP_NORETRY             BIT(17)

/* Watermark modifiers bitmasks */
#define GFP_ATOMIC              BIT(10)
#define GFP_HIGH                BIT(6)
#define GFP_MEMALLOC            BIT(18)
#define GFP_NOMEMALLOC          BIT(20)
#define GFP_WRITE               BIT(13)
#define GFP_NOWARN              BIT(14)
#define GFP_RETRY_MAYFAIL       BIT(15)
#define GFP_COMP                BIT(19)
#define GFP_HARDWALL            BIT(21)
#define GFP_THISNODE            BIT(22)
#define GFP_ACCOUNT             BIT(23)

#define GFP_KERNEL              (GFP_NORMAL)
#define GFP_NOWAIT	            (GFP_KSWAPD_RECLAIM)
#define GFP_NOIO	            (GFP_RECLAIM)
#define GFP_NOFS	            (GFP_RECLAIM | GFP_IO)
#define GFP_USER	            (GFP_RECLAIM | GFP_IO | GFP_FS | GFP_HARDWALL)

#endif  /* __ASSEMBLY__ */
#endif  /* _MM_GFP_H_ */
