/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _ASM_CSKY_BARRIER_H_
#define _ASM_CSKY_BARRIER_H_

#ifndef __ASSEMBLY__

#define nop()	asm volatile ("nop\n":::"memory")

/*
 * bar.brwarws: ordering barrier for all load/store instructions
 *              before/after
 *
 * |31|30 26|25 21|20 16|15  10|9   5|4           0|
 *  1  10000 00000 00000 100001	00001 0 bw br aw ar
 *
 * b: before
 * a: after
 * r: read
 * w: write
 *
 * Here are all combinations:
 *
 * bar.brw
 * bar.br
 * bar.bw
 * bar.arw
 * bar.ar
 * bar.aw
 * bar.brwarw
 * bar.brarw
 * bar.bwarw
 * bar.brwar
 * bar.brwaw
 * bar.brar
 * bar.bwaw
 */
#define __bar_brw()     asm volatile (".long 0x842cc000\n":::"memory")
#define __bar_br()      asm volatile (".long 0x8424c000\n":::"memory")
#define __bar_bw()      asm volatile (".long 0x8428c000\n":::"memory")
#define __bar_arw()     asm volatile (".long 0x8423c000\n":::"memory")
#define __bar_ar()      asm volatile (".long 0x8421c000\n":::"memory")
#define __bar_aw()      asm volatile (".long 0x8422c000\n":::"memory")
#define __bar_brwarw()  asm volatile (".long 0x842fc000\n":::"memory")
#define __bar_brarw()   asm volatile (".long 0x8427c000\n":::"memory")
#define __bar_bwarw()   asm volatile (".long 0x842bc000\n":::"memory")
#define __bar_brwar()   asm volatile (".long 0x842dc000\n":::"memory")
#define __bar_brwaw()   asm volatile (".long 0x842ec000\n":::"memory")
#define __bar_brar()    asm volatile (".long 0x8425c000\n":::"memory")
#define __bar_bwaw()    asm volatile (".long 0x842ac000\n":::"memory")

#define __smp_mb()      __bar_brwarw()
#define __smp_rmb()     __bar_brar()
#define __smp_wmb()     __bar_bwaw()

/*
 * sync:        completion barrier, all sync.xx instructions
 *              guarantee the last response recieved by bus transaction
 *              made by ld/st instructions before sync.s
 * sync.s:      inherit from sync, but also shareable to other cores
 * sync.i:      inherit from sync, but also flush cpu pipeline
 * sync.is:     the same with sync.i + sync.s
 */
#define mb()    asm volatile ("sync\n":::"memory")

#include <asm-generic/barrier.h>

#endif /* __ASSEMBLY__ */
#endif /* _ASM_CSKY_BARRIER_H_ */
