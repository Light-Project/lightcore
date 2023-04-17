/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_GFP_H_
#define _MM_GFP_H_

#include <types.h>
#include <bits.h>

enum gfp_flags {
    __GFP_DMA = 0,
    __GFP_DMA32,
    __GFP_HIGHMEM,
    __GFP_MOVABLE,
    __GFP_IO,
    __GFP_FS,
    __GFP_ZERO,
    __GFP_NOFAIL,
    __GFP_NORETRY,
    __GFP_ATOMIC,
    __GFP_HIGH,
    __GFP_MEMALLOC,
    __GFP_NOMEMALLOC,
    __GFP_WRITE,
    __GFP_NOWARN,
    __GFP_RETRY_MAYFAIL,
    __GFP_COMP,
    __GFP_HARDWALL,
    __GFP_THISNODE,
    __GFP_ACCOUNT,
};

/**
 * Physical region bitmasks
 */
#define GFP_DMA                 BIT(__GFP_DMA)
#define GFP_DMA32               BIT(__GFP_DMA32)
#define GFP_HIGHMEM             BIT(__GFP_HIGHMEM)
#define GFP_MOVABLE             BIT(__GFP_MOVABLE)
#define GFP_REGION_MASK         BIT_RANGE(__GFP_MOVABLE, __GFP_DMA)

/**
 * Reclaim modifier bitmasks
 */
#define GFP_IO                  BIT(__GFP_IO)
#define GFP_FS                  BIT(__GFP_FS)
#define GFP_ZERO                BIT(__GFP_ZERO)
#define GFP_NOFAIL              BIT(__GFP_NOFAIL)
#define GFP_NORETRY             BIT(__GFP_NORETRY)
#define GFP_RECLAIM_MASK        BIT_RANGE(__GFP_NORETRY, __GFP_IO)

/**
 * Watermark modifiers bitmasks
 */
#define GFP_ATOMIC              BIT(__GFP_ATOMIC)
#define GFP_HIGH                BIT(__GFP_HIGH)
#define GFP_MEMALLOC            BIT(__GFP_MEMALLOC)
#define GFP_NOMEMALLOC          BIT(__GFP_NOMEMALLOC)

#define GFP_WRITE               BIT(__GFP_WRITE)
#define GFP_NOWARN              BIT(__GFP_NOWARN)
#define GFP_RETRY_MAYFAIL       BIT(__GFP_RETRY_MAYFAIL)
#define GFP_COMP                BIT(__GFP_COMP)
#define GFP_HARDWALL            BIT(__GFP_HARDWALL)
#define GFP_THISNODE            BIT(__GFP_THISNODE)
#define GFP_ACCOUNT             BIT(__GFP_ACCOUNT)

#define GFP_KERNEL              (GFP_IO | GFP_FS)
#define GFP_USER                (GFP_IO | GFP_FS | GFP_HARDWALL)

#endif /* _MM_GFP_H_ */
