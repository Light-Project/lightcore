/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_GFP_H_
#define _MM_GFP_H_

#include <types.h>
#include <bits.h>

#ifndef __ASSEMBLY__

/* Physical address region bitmasks */
#define GFP_DMA                 BIT(0)
#define GFP_DMA32               BIT(1)
#define GFP_HIGHMEM             BIT(6)
#define GFP_MOVABLE             BIT(7)

#define GFP_NORMAL              0x00
#define GFP_REGION_MASK         0xff

/* Reclaim modifier bitmasks */
#define GFP_IO                  BIT(7)
#define GFP_FS                  BIT(8)
#define GFP_DIRECT_RECLAIM      BIT(11)
#define GFP_KSWAPD_RECLAIM      BIT(12)
#define GFP_NOFAIL              BIT(16)
#define GFP_NORETRY             BIT(17)

#define GFP_ZERO                BIT(9)

/* Watermark modifiers bitmasks */
#define WKM_ATOMIC              BIT(10)
#define WKM_HIGH                BIT(6)
#define WKM_MEMALLOC            BIT(18)
#define WKM_NOMEMALLOC          BIT(20)

#define GFP_WRITE               BIT(13)
#define GFP_NOWARN              BIT(14)
#define GFP_RETRY_MAYFAIL       BIT(15)

#define GFP_COMP                BIT(19)

#define GFP_HARDWALL            BIT(21)
#define GFP_THISNODE            BIT(22)
#define GFP_ACCOUNT             BIT(23)

#define GFP_ATOMIC              0
#define GFP_KERNEL              GFP_NORMAL
#define GFP_KERNEL_ACCOUNT  (GFP_KERNEL | __GFP_ACCOUNT)
#define GFP_NOWAIT	        (__GFP_KSWAPD_RECLAIM)
#define GFP_NOIO	        (__GFP_RECLAIM)
#define GFP_NOFS	        (__GFP_RECLAIM | __GFP_IO)
#define GFP_USER	        (__GFP_RECLAIM | __GFP_IO | __GFP_FS | __GFP_HARDWALL)

#endif /* __ASSEMBLY__ */
#endif /* _MM_GFP_H_ */
