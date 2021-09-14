/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BARRIER_H_
#define _ASM_GENERIC_BARRIER_H_

#ifndef nop
#define nop()   asm volatile ("nop")
#endif

/* Read and write memory barrier */
#ifndef mb
#define mb()        barrier()
#endif

/* Read memory barrier */
#ifndef rmb
#define rmb()       mb()
#endif

/* Write memory barrier */
#ifndef wmb
#define wmb()       mb()
#endif

/* Dma read memory barrier */
#ifndef dma_rmb
#define dma_rmb()   rmb()
#endif

/* Dma write memory barrier */
#ifndef dma_wmb
#define dma_wmb()   wmb()
#endif

#ifndef __smp_mb
#define __smp_mb()  mb()
#endif

#ifndef __smp_rmb
#define __smp_rmb() rmb()
#endif

#ifndef __smp_wmb
#define __smp_wmb() wmb()
#endif

#ifdef CONFIG_SMP

/* SMP read and write memory barrier */
#ifndef smp_mb
#define smp_mb()    __smp_mb()
#endif

/* SMP read memory barrier */
#ifndef smp_rmb
#define smp_rmb()   __smp_rmb()
#endif

/* SMP write memory barrier */
#ifndef smp_wmb
#define smp_wmb()   __smp_wmb()
#endif

#else /* !CONFIG_SMP */

/* SMP read and write memory barrier */
#ifndef smp_mb
#define smp_mb()    barrier()
#endif

/* SMP read memory barrier */
#ifndef smp_rmb
#define smp_rmb()   barrier()
#endif

/* SMP write memory barrier */
#ifndef smp_wmb
#define smp_wmb()   barrier()
#endif

#endif  /* CONFIG_SMP */
#endif  /* _ASM_GENERIC_BARRIER_H_ */
