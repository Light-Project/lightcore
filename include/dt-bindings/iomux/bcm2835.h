/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DT_BINDINGS_IOMUX_BCM2835_H_
#define _DT_BINDINGS_IOMUX_BCM2835_H_

/* brcm,function property */
#define BCM2835_FSEL_GPIO_IN	0
#define BCM2835_FSEL_GPIO_OUT	1
#define BCM2835_FSEL_ALT5	2
#define BCM2835_FSEL_ALT4	3
#define BCM2835_FSEL_ALT0	4
#define BCM2835_FSEL_ALT1	5
#define BCM2835_FSEL_ALT2	6
#define BCM2835_FSEL_ALT3	7

/* brcm,pull property */
#define BCM2835_PUD_OFF		0
#define BCM2835_PUD_DOWN	1
#define BCM2835_PUD_UP		2 

#endif  /* _DT_BINDINGS_IOMUX_BCM2835_H_ */
