/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/pic.h
 *  x86 pic register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version 
 * 
 */

#ifndef _CHIP_X86_PIC_H_
#define _CHIP_X86_PIC_H_

#include <types.h>

#define PORT_PIC1           0x20    /* IO base address for master PIC */
#define PORT_PIC2           0xa0    /* IO base address for slave PIC */

#define PORT_PIC1_CMD       (PORT_PIC1 + 0x00)
#define PORT_PIC1_DATA      (PORT_PIC1 + 0x01)
#define PORT_PIC2_CMD       (PORT_PIC2 + 0x00)
#define PORT_PIC2_DATA      (PORT_PIC2 + 0x01)

#define ICW1_ICW4	        0x01	/* ICW4 needed                  */
#define ICW1_SINGLE	        0x02	/* Single (cascade) mode        */
#define ICW1_INTERVAL4	    0x04	/* Call address interval 4 (8)  */
#define ICW1_LEVEL	        0x08	/* Level triggered (edge) mode  */
#define ICW1_INIT	        0x10	/* Initialization - required!   */

#define ICW4_8086	        0x01	/* 8086/88 (MCS-80/85) mode     */
#define ICW4_AUTO	        0x02	/* Auto (normal) EOI            */
#define ICW4_BUF_SLAVE	    0x08	/* Buffered mode/slave          */
#define ICW4_BUF_MASTER	    0x0C	/* Buffered mode/master         */
#define ICW4_SFNM	        0x10	/* Special fully nested (not)   */

/*      PORT_PIC1 bitdefs                                                               */
#define PIC1_IRQ0           (1<<0)  /* System Timer                                     */
#define PIC1_IRQ1           (1<<1)  /* Keyboard                                         */
#define PIC1_IRQ2           (1<<2)  /* Cascade Controller                               */
#define PIC1_IRQ3           (1<<3)  /* COM 2 and 4                                      */
#define PIC1_IRQ4           (1<<4)  /* COM 1 and 3                                      */
#define PIC1_IRQ5           (1<<5)  /* Sound, Parallel port 2                           */
#define PIC1_IRQ6           (1<<6)  /* Floppy                                           */
#define PIC1_IRQ7           (1<<7)  /* Parallel Port 1                                  */
/*      PORT_PIC2 bitdefs                                                               */
#define PIC2_IRQ8           (1<<8)  /* Real-time clock                                  */
#define PIC2_IRQ9           (1<<9)  /* Redirected IRQ 2, Open, Network Available        */
#define PIC2_IRQ10          (1<<10) /* Open                                             */
#define PIC2_IRQ11          (1<<11) /* Open, SCSI, Video                                */
#define PIC2_IRQ12          (1<<12) /* Open, PS2                                        */           
#define PIC2_IRQ13          (1<<13) /* Coprocessor                                      */
#define PIC2_IRQ14          (1<<14) /* Open, Primary hard drive, Hard drive controller  */
#define PIC2_IRQ15          (1<<15) /* Open, 2nd hard drive (secondary)                 */

#define PIC_EOI             0x20    /* End-of-interrupt command code */
#define PIC_READ_IRR        0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR        0x0b    /* OCW3 irq service next CMD read */

#define PIC_IRQMASK_DEFAULT ((uint16_t)~PIC1_IRQ2)

#endif
