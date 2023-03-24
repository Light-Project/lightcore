/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_GFP_H_
#define _MM_GFP_H_

#include <types.h>
#include <bits.h>

enum gfp_flags {
    __GFP_DMA               = 0,
    __GFP_DMA32             = 1,
    __GFP_HIGHMEM           = 2,
    __GFP_MOVABLE           = 3,
    __GFP_IO                = 4,
    __GFP_FS                = 5,
    __GFP_ZERO              = 6,
    __GFP_NOFAIL            = 7,
    __GFP_NORETRY           = 8,
    __GFP_ATOMIC            = 9,
    __GFP_HIGH              = 10,
    __GFP_MEMALLOC          = 11,
    __GFP_NOMEMALLOC        = 12,
    __GFP_WRITE             = 13,
    __GFP_NOWARN            = 14,
    __GFP_RETRY_MAYFAIL     = 15,
    __GFP_COMP              = 16,
    __GFP_HARDWALL          = 17,
    __GFP_THISNODE          = 18,
    __GFP_ACCOUNT           = 19,
};

/**
 * Physical region bitmasks
 *
 *
 */
#define GFP_DMA                 BIT(__GFP_DMA)
#define GFP_DMA32               BIT(__GFP_DMA32)
#define GFP_HIGHMEM             BIT(__GFP_HIGHMEM)
#define GFP_MOVABLE             BIT(__GFP_MOVABLE)
#define GFP_REGION_MASK         BIT_RANGE(__GFP_MOVABLE, __GFP_DMA)

/**
 * Reclaim modifier bitmasks
 *
 *
 */
#define GFP_IO                  BIT(__GFP_IO)
#define GFP_FS                  BIT(__GFP_FS)
#define GFP_ZERO                BIT(__GFP_ZERO)
#define GFP_NOFAIL              BIT(__GFP_NOFAIL)
#define GFP_NORETRY             BIT(__GFP_NORETRY)
#define GFP_RECLAIM_MASK        BIT_RANGE(__GFP_NORETRY, __GFP_IO)

/**
 * Watermark modifiers bitmasks
 *
 *
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
